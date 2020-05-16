#pragma once

#include <string>
#include <vector>

namespace xml {

struct role {
  std::string role_arn;
  std::string principal_arn;
};

std::vector<std::string> split(const std::string &role);

std::vector<role> get_roles(const std::string &assertion);

}  // namespace xml
