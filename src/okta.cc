#include "okta.h"

#include "cli.h"
#include "unescape.h"

using json = nlohmann::json;

namespace okta {

json auth(const std::string &username, const std::string &password,
          const std::string &org) {
  json auth;
  auth["username"] = username;
  auth["password"] = password;
  std::string payload = auth.dump();
  std::string url = "https://" + org + ".okta.com/api/v1/authn";
  std::string buffer;

  int res = curl::post(url, payload, buffer);
  if (res != 0) throw(std::runtime_error("Okta authentication failed"));

  std::cout << "Okta authentication complete." << std::endl;

  return json::parse(buffer);
}

std::string wait_for_push(const std::string &next_url,
                          const std::string &payload) {
  std::string buffer;
  curl::post(next_url, payload, buffer);
  json response = json::parse(buffer);
  if (response["status"] == "SUCCESS") {
    std::cout << "Okta Push confirmed." << std::endl;
    return response["sessionToken"];
  }

  if (response["factorStatus"] == "TIMEOUT")
    throw(std::runtime_error("MFA response timed out"));

  if (response["factorStatus"] == "REJECTED")
    throw(std::runtime_error("MFA push rejected"));

  return wait_for_push(next_url, payload);
}

std::string verify_push(const json &factor, const std::string &state_token) {
  json body;
  body["stateToken"] = state_token;
  std::string payload = body.dump();
  std::string url = factor["_links"]["verify"]["href"];
  std::string buffer;

  curl::post(url, payload, buffer);
  std::cout << "Okta Push initiated. Waiting for response..." << std::endl;

  json response = json::parse(buffer);

  std::string next_url = response["_links"]["next"]["href"];

  return wait_for_push(next_url, payload);
}

std::string verify_mfa(const json &factors, const std::string &state_token) {
  for (const auto &factor : factors)
    if (factor["factorType"] == "push") return verify_push(factor, state_token);

  throw(std::runtime_error("No supported factors"));
}

std::string get_session_id(const std::string &session_token,
                           const std::string &org) {
  json body;
  body["sessionToken"] = session_token;
  std::string payload = body.dump();
  std::string url = "https://" + org + ".okta.com/api/v1/sessions";
  std::string buffer;

  curl::post(url, payload, buffer);
  json response = json::parse(buffer);

  return response["id"];
}

std::vector<app> get_apps(const std::string &session_id,
                          const std::string &org) {
  std::vector<app> results;
  std::string url = "https://" + org + ".okta.com/api/v1/users/me/appLinks";
  std::string buffer;

  curl::get(url, buffer, session_id);
  json response = json::parse(buffer);

  for (auto &entry : response)
    if (entry["appName"] == "amazon_aws") {
      app app{entry["label"], entry["linkUrl"]};
      results.push_back(app);
    }

  return results;
}

std::string get_saml_assertion(const std::string &app_link,
                               const std::string &session_id) {
  std::string buffer;

  curl::get(app_link, buffer, session_id);

  std::regex regex(
      "<input name=\"SAMLResponse\" type=\"hidden\" value=\"(.*?)\"");

  std::smatch matches;
  bool res = std::regex_search(buffer, matches, regex);
  if (!res) throw(std::runtime_error("SAMLResponse match failed"));

  return matches[1];
}

std::string main(const std::string &username, const std::string &password,
                 const std::string &org) {
  json response = okta::auth(username, password, org);
  std::string state_token = response["stateToken"];
  json factors = response["_embedded"]["factors"];

  std::string session_token = okta::verify_mfa(factors, state_token);

  std::string session_id = okta::get_session_id(session_token, org);

  std::vector<okta::app> apps = okta::get_apps(session_id, org);
  okta::app app = cli::select_okta_app(apps);

  std::string saml = okta::get_saml_assertion(app.link, session_id);

  unescape(saml);
  return saml;
}

}  // namespace okta