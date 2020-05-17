#include <string>

#include "okta.h"
#include "xml.h"

namespace cli {

struct settings {
  std::string username;
  std::string password;
  std::string org;
  std::string profile_name;
};

settings main(int argc, char *argv[]);

okta::app select_okta_app(std::vector<okta::app> apps);
xml::role select_role(std::vector<xml::role> roles);

}  // namespace cli