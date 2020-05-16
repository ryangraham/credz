#include "cli.h"

#include <termios.h>
#include <unistd.h>

#include <boost/program_options.hpp>
#include <fstream>

#include "okta.h"
#include "path.h"
#include "version.h"
#include "xml.h"

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

settings main(int argc, char *argv[]) {
  std::string config_file;
  po::options_description desc("Allowed options");
  desc.add_options()("version,v", "print version string")(
      "help", "produce help message")(
      "config,c",
      po::value<std::string>(&config_file)->default_value("~/.credz"),
      "Path to config file");

  std::string org;
  std::string username;
  std::string password;
  po::options_description config("Configuration");
  config.add_options()("Settings.organization", po::value<std::string>(&org),
                       "Okta organization")("Settings.username",
                                            po::value<std::string>(&username));

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

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

  password_prompt(password);

  return settings{username, password, org};
}

}  // namespace cli