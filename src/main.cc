
#include <nlohmann/json.hpp>
#include <string>

#include "aws.h"
#include "base64.h"
#include "cli.h"
#include "ini.h"
#include "okta.h"
#include "path.h"
#include "unescape.h"
#include "xml.h"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
  // NOLINTNEXTLINE
  curl_global_init(CURL_GLOBAL_ALL);

  auto settings = cli::main(argc, argv);

  json response =
      okta::auth(settings.username, settings.password, settings.org);
  std::string state_token = response["stateToken"];
  json factors = response["_embedded"]["factors"];

  std::string session_token = okta::verify_mfa(factors, state_token);

  std::string session_id = okta::get_session_id(session_token, settings.org);

  std::vector<okta::app> apps = okta::get_apps(session_id, settings.org);
  okta::app app = cli::select_okta_app(apps);

  std::string saml = okta::get_saml_assertion(app.link, session_id);

  std::string unescaped_saml = unescape(saml);

  std::string decoded_saml = base64::decode(unescaped_saml);

  std::vector<xml::role> roles = xml::get_roles(decoded_saml);
  xml::role role = cli::select_role(roles);

  auto profile = aws::assume_role_with_saml(unescaped_saml, role.principal_arn,
                                            role.role_arn);
  profile.name = "credz";

  auto tree = ini::load_file();
  ini::put_profile(tree, profile);

  curl_global_cleanup();
  return 0;
}
