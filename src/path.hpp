#include <string>
#include <wordexp.h>

namespace path {

void expand(std::string &path) {
  std::string result;
  wordexp_t exp_result;
  wordexp(path.c_str(), &exp_result, 0);
  path = exp_result.we_wordv[0];
  wordfree(&exp_result);
}

} // namespace path