
#include "cli.h"

#include <keyring.h>
#include <termios.h>
#include <unistd.h>

#include <boost/program_options.hpp>
#include <fstream>

#include "ini.h"
#include "okta.h"
#include "path.h"
#include "version.h"
#include "xml.h"

#define DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
#define DOCTEST_CONFIG_IMPLEMENT

#include <doctest/doctest.h>

namespace po = boost::program_options;

namespace cli {

void disable_echo() {
  struct termios tty = {};

  if (tcgetattr(STDIN_FILENO, &tty) != 0)
    throw(std::runtime_error("Failed to fetch TTY"));

  // NOLINTNEXTLINE
  tty.c_lflag &= ~ECHO;

  if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0)
    throw(std::runtime_error("Failed to disable echo"));
}

void enable_echo() {
  struct termios tty = {};

  if (tcgetattr(STDIN_FILENO, &tty) != 0)
    throw(std::runtime_error("Failed to fetch TTY"));

  // NOLINTNEXTLINE
  tty.c_lflag |= ECHO;

  if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0)
    throw(std::runtime_error("Failed to enable echo"));
}

int password_prompt(std::string &password) {
  disable_echo();

  std::cout << "Password: ";
  std::cin >> password;

  enable_echo();

  std::cout << std::endl;

  return 0;
}

void username_prompt(std::string &username) {
  std::cout << "Username: ";
  std::cin >> username;
}

void org_prompt(std::string &org) {
  std::cout << "Okta Org: ";
  std::cin >> org;
}

xml::role select_role(std::vector<xml::role> roles) {
  if (roles.size() == 1) return roles[0];

  std::cout << std::endl;

  int menu_count = 1;
  for (auto &role : roles) {
    std::cout << menu_count << ". " << role.role_arn << std::endl;
    menu_count++;
  }
  std::cout << std::endl << "Enter a number to select a role: ";

  int number;
  std::cin >> number;

  if (number > roles.size())
    throw(std::runtime_error("Selection out of range"));

  xml::role role = roles[number - 1];

  std::cout << std::endl
            << "You selected " << number << " - " << role.role_arn << std::endl
            << std::endl;

  return role;
}

okta::app select_okta_app(std::vector<okta::app> apps) {
  if (apps.size() == 1) return apps[0];

  std::cout << std::endl;

  int menu_count = 1;
  for (auto &app : apps) {
    std::cout << menu_count << ". " << app.label << std::endl;
    menu_count++;
  }
  std::cout << std::endl << "Enter a number to select an app: ";

  int number;
  std::cin >> number;

  if (number > apps.size()) throw(std::runtime_error("Selection out of range"));

  okta::app app = apps[number - 1];

  std::cout << std::endl
            << "You selected " << number << " - " << app.label << std::endl
            << std::endl;

  return app;
}

void get_password(const std::string &org, const std::string &username,
                  std::string &password, bool enable_keychain) {
  if (!enable_keychain) {
    password_prompt(password);
    return;
  }

  std::string service_name = "credz";
  std::string account_name = org + "-" + username;
  int success = keyring::get_password(service_name, account_name, password);

  if (success != 0) {
    password_prompt(password);
    keyring::set_password(service_name, account_name, password);
  }
}

void doctest(int argc, char **argv) {
  doctest::Context context(argc, argv);
  int test_result = context.run();

  exit(test_result);
}

settings main(int argc, char *argv[]) {
  std::string config_file;
  std::string profile_name;
  bool run_tests = false;
  po::options_description desc("Allowed options");
  desc.add_options()("version,v", "print version string")(
      "help", "produce help message")(
      "config,c",
      po::value<std::string>(&config_file)->default_value("~/.credz"),
      "Path to config file")(
      "profile,p",
      po::value<std::string>(&profile_name)->default_value("credz"),
      "Profile name to create in ~/aws/credentials")(
      "test,t", po::bool_switch(&run_tests), "Run doctests");

  std::string org;
  std::string username;
  std::string password;
  bool enable_keychain;
  po::options_description config("Configuration");
  config.add_options()("Okta.organization", po::value<std::string>(&org),
                       "Okta organization")("Okta.username",
                                            po::value<std::string>(&username))(
      "Okta.enable_keychain",
      po::value<bool>(&enable_keychain)->default_value(true));

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (run_tests) doctest(argc, argv);

  if (vm.count("help") != 0u) {
    std::cout << desc << std::endl;
    exit(1);
  }

  if (vm.count("version") != 0u) {
    std::cout << VERSION << std::endl;
    exit(0);
  }

  path::expand(config_file);
  std::ifstream file(config_file, std::ios::in);
  if (file) {
    store(parse_config_file(file, config), vm);
    notify(vm);
  }

  if (vm.count("Okta.organization") == 0u) org_prompt(org);

  if (vm.count("Okta.username") == 0u) username_prompt(username);

  ini::write_config(org, username, config_file);

  get_password(org, username, password, enable_keychain);

  return settings{username, password, org, profile_name};
}

}  // namespace cli