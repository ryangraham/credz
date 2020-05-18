#include <string>

#include "base64/polfosol_mod.h"
#include "doctest/doctest.h"

namespace base64 {

inline std::string decode(const std::string &data) {
  return b64decode_mod(data.data(), data.length());
}

}  // namespace base64

TEST_CASE("testing base64 decode") {
  std::string input = "cm9mbG1hbw==";
  std::string expected = "roflmao";

  CHECK(base64::decode(input) == expected);
}
