#pragma once
#include <string>

#ifdef __APPLE__

namespace keychain {

bool set_password(const std::string service_name, const std::string &account,
                  const std::string &password);
bool get_password(const std::string service_name, const std::string &account,
                  std::string &password);
}  // namespace keychain

#endif
