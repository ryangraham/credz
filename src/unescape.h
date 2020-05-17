#include <boost/regex.hpp>
#include <iostream>
#include <string>

#include "doctest/doctest.h"

/*
 * Unescape HTML entities
 *
 * Example: "&#x3D;" becomes "="
 *
 * https://en.wikipedia.org/wiki/Numeric_character_reference
 */
inline std::string unescape(const std::string &input) {
  const int BASE_HEX = 16;
  std::string output;
  boost::regex re("&#x([a-zA-Z0-9][a-zA-Z0-9]);");
  const std::vector<int> submatches = {-1, 1};
  boost::sregex_token_iterator end;

  boost::sregex_token_iterator token(input.begin(), input.end(), re,
                                     submatches);

  for (bool is_ncr = false; token != end; ++token, is_ncr = !is_ncr) {
    if (is_ncr) {
      unsigned int d = std::stoul("0x" + *token, nullptr, BASE_HEX);
      output += static_cast<char>(d);
    } else
      output += *token;
  }

  return output;
}

TEST_CASE("testing html unescape") {
  std::string input = "DEADBEEF&#x2B;123456ABCDEF&#x3D;&#x3D;123&#x3D;";
  std::string expected = "DEADBEEF+123456ABCDEF==123=";

  CHECK(unescape(input) == expected);
}
