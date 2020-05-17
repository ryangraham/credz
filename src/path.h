#pragma once

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <wordexp.h>

#include <string>

#include "doctest/doctest.h"

namespace path {

inline void expand(std::string &path) {
  std::string result;
  wordexp_t exp_result;
  wordexp(path.c_str(), &exp_result, 0);
  path = exp_result.we_wordv[0];
  wordfree(&exp_result);
}

inline std::string get_home_dir() {
  char *home_dir;

  home_dir = getenv("HOME");

  if (home_dir == nullptr) home_dir = getpwuid(getuid())->pw_dir;

  return std::string(home_dir);
}

}  // namespace path

TEST_CASE("testing path expansion") {
  std::string home_dir = path::get_home_dir();
  std::string expansion = "~";
  path::expand(expansion);

  CHECK(expansion == home_dir);
}
