
#include <string>

#include "aws.h"
#include "base64.h"
#include "cli.h"
#include "ini.h"
#include "okta.h"
#include "xml.h"

int main(int argc, char *argv[]) {
  // NOLINTNEXTLINE
  curl_global_init(CURL_GLOBAL_ALL);

  auto settings = cli::main(argc, argv);

  std::string saml =
      okta::main(settings.username, settings.password, settings.org);

  std::string decoded_saml = base64::decode(saml);
  std::vector<xml::role> roles = xml::get_roles(decoded_saml);
  xml::role role = cli::select_role(roles);

  auto profile =
      aws::assume_role_with_saml(saml, role.principal_arn, role.role_arn);
  profile.name = "credz";

  auto tree = ini::load_file();
  ini::put_profile(tree, profile);

  curl_global_cleanup();
  return 0;
}
