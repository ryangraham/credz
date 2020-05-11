#include <iostream>
#include <regex>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

#include "base64.hpp"
#include "curl.hpp"
#include "unescape.hpp"
#include "xml.hpp"
#include "zlib.hpp"

using json = nlohmann::json;

json okta_auth(const std::string &username, const std::string &password, const std::string &org)
{
    json auth;
    auth["username"] = username;
    auth["password"] = password;
    std::string payload = auth.dump();
    std::string url = "https://" + org + ".okta.com/api/v1/authn";
    std::string buffer;

    int res = curl::post(url, payload, buffer);
    if (res != 0)
        throw(std::runtime_error("Okta authentication failed"));

    return json::parse(buffer);
}

int password_prompt(std::string &password)
{
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) != 0)
        return -1;

    tty.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0)
        return -1;

    std::cout << "Password: ";
    std::cin >> password;

    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);

    return 0;
}

void username_prompt(std::string &username)
{
    std::cout << "Username: ";
    std::cin >> username;
}

void org_prompt(std::string &org)
{
    std::cout << "Okta Org: ";
    std::cin >> org;
}

std::string wait_for_push(const std::string &next_url, const std::string &payload)
{
    std::string buffer;
    curl::post(next_url, payload, buffer);
    json response = json::parse(buffer);
    if (response["status"] == "SUCCESS")
        return response["sessionToken"];

    if (response["factorStatus"] == "TIMEOUT")
        throw(std::runtime_error("MFA response timed out"));

    if (response["factorStatus"] == "REJECTED")
        throw(std::runtime_error("MFA push rejected"));

    return wait_for_push(next_url, payload);
}

std::string verify_push(const json &factor, const std::string &state_token)
{
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

std::string verify_mfa(const json &factors, const std::string &state_token)
{
    for (auto &factor : factors)
        if (factor["factorType"] == "push")
            return verify_push(factor, state_token);

    throw(std::runtime_error("No supported factors"));
}

std::string get_session_id(const std::string &session_token, const std::string &org)
{
    json body;
    body["sessionToken"] = session_token;
    std::string payload = body.dump();
    std::string url = "https://" + org + ".okta.com/api/v1/sessions";
    std::string buffer;

    curl::post(url, payload, buffer);
    json response = json::parse(buffer);

    return response["id"];
}

std::string get_app_link(const std::string &session_id, const std::string &org)
{
    std::string url = "https://" + org + ".okta.com/api/v1/users/me/appLinks";
    std::string buffer;

    curl::get(url, buffer, session_id);
    json response = json::parse(buffer);

    // TODO: This assumes only one, but there can be many
    for (auto &app : response)
        if (app["appName"] == "amazon_aws")
            return app["linkUrl"];

    throw(std::runtime_error("No AWS apps configured in Okta"));
}

std::string get_saml_assertion(const std::string &app_link, const std::string &session_id)
{
    std::string buffer;

    curl::get(app_link, buffer, session_id);

    std::regex regex("<input name=\"SAMLResponse\" type=\"hidden\" value=\"(.*?)\"");

    std::smatch matches;
    int res = std::regex_search(buffer, matches, regex);
    if (!res)
        throw(std::runtime_error("SAMLResponse match failed"));

    return matches[1];
}

int main(void)
{

    curl_global_init(CURL_GLOBAL_ALL);

    std::string org = "";
    std::string username = "";
    std::string password = "";

    org_prompt(org);
    username_prompt(username);
    password_prompt(password);
    std::cout << std::endl;

    json response = okta_auth(username, password, org);
    std::string state_token = response["stateToken"];
    json factors = response["_embedded"]["factors"];

    std::string session_token = verify_mfa(factors, state_token);

    std::string session_id = get_session_id(session_token, org);

    std::string app_link = get_app_link(session_id, org);

    std::string saml = get_saml_assertion(app_link, session_id);

    std::string unescaped = unescape(saml);

    std::string decoded = base64::decode(unescaped);

    std::vector<std::vector<std::string>> roles = get_roles(decoded);
    for (auto &role : roles)
        for (auto &arn : role)
            std::cout << arn << std::endl;

    curl_global_cleanup();
    return 0;
}
