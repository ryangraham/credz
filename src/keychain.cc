#ifdef __APPLE__

#include <Security/Security.h>

#include <iostream>
#include <string>

namespace keychain {

bool set_password(const std::string service_name, const std::string &account,
                  const std::string &password) {
  auto ret = SecKeychainAddGenericPassword(
      NULL, service_name.size(), service_name.c_str(), account.size(),
      account.c_str(), password.size(), password.c_str(), NULL);

  return ret == 0;
}

bool get_password(const std::string service_name, const std::string &account,
                  std::string &password) {
  UInt32 pw_len;
  char *buffer;

  auto ret = SecKeychainFindGenericPassword(
      NULL, service_name.size(), service_name.c_str(), account.size(),
      account.c_str(), &pw_len, (void **)&buffer, NULL);

  if (ret == 0) {
    password = std::string(buffer, pw_len);
    SecKeychainItemFreeContent(NULL, buffer);
  }

  return ret == 0;
}

}  // namespace keychain

#endif
