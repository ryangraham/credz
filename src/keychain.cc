#ifdef __APPLE__

#include <Security/Security.h>

#include <iostream>
#include <string>

namespace keychain {

bool set_password(const std::string &service_name, const std::string &account,
                  const std::string &password) {
  auto ret = SecKeychainAddGenericPassword(
      nullptr, service_name.size(), service_name.c_str(), account.size(),
      account.c_str(), password.size(), password.c_str(), nullptr);

  if (ret == 0) std::cout << "Password written to keychain." << std::endl;

  return ret == 0;
}

bool get_password(const std::string &service_name, const std::string &account,
                  std::string &password) {
  UInt32 pw_len;
  char *buffer;

  auto ret = SecKeychainFindGenericPassword(
      nullptr, service_name.size(), service_name.c_str(), account.size(),
      account.c_str(), &pw_len, reinterpret_cast<void **>(&buffer), nullptr);

  if (ret == 0) {
    password = std::string(buffer, pw_len);
    SecKeychainItemFreeContent(nullptr, buffer);
  }

  return ret == 0;
}

}  // namespace keychain

#endif
