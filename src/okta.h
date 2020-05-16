#pragma once

#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>

#include "curl.h"

using json = nlohmann::json;

namespace okta {

struct app {
  std::string label;
  std::string link;
};

std::string main(const std::string &username, const std::string &password,
          const std::string &org);

json auth(const std::string &username, const std::string &password,
          const std::string &org);

std::string wait_for_push(const std::string &next_url,
                          const std::string &payload);

std::string verify_push(const json &factor, const std::string &state_token);

std::string verify_mfa(const json &factors, const std::string &state_token);

std::string get_session_id(const std::string &session_token,
                           const std::string &org);

std::vector<app> get_apps(const std::string &session_id,
                          const std::string &org);

std::string get_saml_assertion(const std::string &app_link,
                               const std::string &session_id);

}  // namespace okta