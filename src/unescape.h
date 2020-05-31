#include <iostream>
#include <string>
#include <string_view>

#include "doctest/doctest.h"

inline void replace(std::string& input, std::string_view from,
                    std::string_view to) {
  while (true) {
    size_t start_pos = input.find(from);
    if (start_pos == std::string::npos) break;
    input.replace(start_pos, from.length(), to);
  }
}

inline void unescape(std::string& input) {
  constexpr std::string_view equals = "&#x3d;";
  constexpr std::string_view plus = "&#x2b;";
  replace(input, equals, "=");
  replace(input, plus, "+");
}

TEST_CASE("testing html unescape") {
  std::string input = "DEADBEEF&#x2b;123456ABCDEF&#x3d;&#x3d;123&#x3d;";
  std::string expected = "DEADBEEF+123456ABCDEF==123=";

  unescape(input);
  CHECK(input == expected);
}
