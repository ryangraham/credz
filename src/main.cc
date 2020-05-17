#define DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <string>

#include "aws.h"
#include "base64.h"
#include "cli.h"
#include "ini.h"
#include "okta.h"
#include "xml.h"

class dt_removed {
  std::vector<char*> vec;

 public:
  dt_removed(char** argv_in) {
    for (; *argv_in; ++argv_in)
      if (strncmp(*argv_in, "--dt-", strlen("--dt-")) != 0)
        vec.push_back(*argv_in);
    vec.push_back(NULL);
  }

  int argc() { return static_cast<int>(vec.size()) - 1; }
  char** argv() { return &vec[0]; }
};

int program(int argc, char* argv[]) {
  // NOLINTNEXTLINE
  curl_global_init(CURL_GLOBAL_ALL);

  auto settings = cli::main(argc, argv);

  std::string saml =
      okta::main(settings.username, settings.password, settings.org);

  std::string decoded_saml = base64::decode(saml);
  std::vector<xml::role> roles = xml::get_roles(decoded_saml);
  xml::role role = cli::select_role(roles);

  auto profile =
      aws::assume_role_with_saml(saml, role.principal_arn, role.role_arn);
  profile.name = settings.profile_name;

  auto tree = ini::load_file();
  ini::put_profile(tree, profile);
  std::cout << "Profile " << profile.name << " written to credentials file."
            << std::endl;

  curl_global_cleanup();

  return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
  doctest::Context context(argc, argv);
  int test_result = context.run();  // run queries, or run tests unless --no-run

  if (context.shouldExit())  // honor query flags and --exit
    return test_result;

  dt_removed args(argv);
  int app_result = program(args.argc(), args.argv());

  return test_result + app_result;  // combine the 2 results
}

int factorial(const int number) {
  return number < 1 ? 1 : number <= 1 ? number : factorial(number - 1) * number;
}

TEST_CASE("testing the factorial function") {
  CHECK(factorial(0) == 1);
  CHECK(factorial(1) == 1);
  CHECK(factorial(2) == 2);
  CHECK(factorial(3) == 6);
  CHECK(factorial(10) == 3628800);
}
