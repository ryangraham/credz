#include <cfg/cfg.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <string>

#include "aws.h"
#include "path.h"

#define AWS_CRED_DIR "~/.aws/"
#define AWS_CRED_FILE "credentials"
#define AWS_CRED_FULL_PATH AWS_CRED_DIR AWS_CRED_FILE

namespace ini {

void put_profile(cfg::ctree root, const aws::profile &profile) {
  root[profile.name]["aws_access_key_id"] = profile.aws_access_key_id;
  root[profile.name]["aws_secret_access_key"] = profile.aws_secret_access_key;
  root[profile.name]["aws_session_token"] = profile.aws_session_token;

  std::string full_path = AWS_CRED_FULL_PATH;
  path::expand(full_path);

  std::string dir_path = AWS_CRED_DIR;
  path::expand(dir_path);

  boost::filesystem::create_directories(dir_path);

  cfg::write_ini(full_path, root);
}

cfg::ctree load_file() {
  cfg::ctree root;

  std::string full_path = AWS_CRED_FULL_PATH;
  path::expand(full_path);

  if (!boost::filesystem::exists(full_path)) return root;

  cfg::read_ini(full_path, root);

  return root;
}

void write_config(const std::string &org, const std::string &username,
                  std::string config_file) {
  path::expand(config_file);
  if (boost::filesystem::exists(config_file)) return;

  cfg::ctree root;
  root["Okta"]["organization"] = org;
  root["Okta"]["username"] = username;
  root["Okta"]["enable_keychain"] = "true";

  cfg::write_ini(config_file, root);
  std::cout << "Organization and username written to configuration file."
            << std::endl;
}

}  // namespace ini
