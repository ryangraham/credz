#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <boost/program_options.hpp>
#include <nlohmann/json.hpp>

#include "aws.hpp"
#include "base64.hpp"
#include "okta.hpp"
#include "path.hpp"
#include "unescape.hpp"
#include "xml.hpp"

using json = nlohmann::json;
namespace po = boost::program_options;

int password_prompt(std::string &password)
{
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) != 0)
        return -1;

    tty.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0)
        return -1;

    std::cout << "Password: ";
    std::cin >> password;

    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);

    return 0;
}

void username_prompt(std::string &username)
{
    std::cout << "Username: ";
    std::cin >> username;
}

void org_prompt(std::string &org)
{
    std::cout << "Okta Org: ";
    std::cin >> org;
}

int main(int argc, char *argv[])
{

    curl_global_init(CURL_GLOBAL_ALL);

    std::string config_file;
    po::options_description desc("Allowed options");
    desc.add_options()("version,v", "print version string")("help", "produce help message")("config,c", po::value<std::string>(&config_file)->default_value("~/.aws-credz"),
                                                                                            "Path to config file");

    std::string org = "";
    std::string username = "";
    std::string password = "";
    po::options_description config("Configuration");
    config.add_options()("Settings.organization", po::value<std::string>(&org),
                         "Okta organization")("Settings.username", po::value<std::string>(&username));

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("version"))
    {
        // TODO: make this a real version after moving to cmake
        std::cout << "0.0.1" << std::endl;
        return 0;
    }

    path::expand(config_file);
    std::ifstream file(config_file, std::ios::in);
    if (file)
    {
        store(parse_config_file(file, config), vm);
        notify(vm);
    }

    if (!vm.count("Settings.organization"))
        org_prompt(org);

    if (!vm.count("Settings.username"))
        username_prompt(username);

    password_prompt(password);
    std::cout << std::endl;

    json response = okta::auth(username, password, org);
    std::string state_token = response["stateToken"];
    json factors = response["_embedded"]["factors"];

    std::string session_token = okta::verify_mfa(factors, state_token);

    std::string session_id = okta::get_session_id(session_token, org);

    std::string app_link = okta::get_app_link(session_id, org);

    std::string saml = okta::get_saml_assertion(app_link, session_id);

    std::string unescaped = unescape(saml);

    std::string decoded = base64::decode(unescaped);

    std::vector<std::vector<std::string>> roles = get_roles(decoded);
    for (auto &role : roles)
        for (auto &arn : role)
            std::cout << arn << std::endl;

    std::string principal_arn = roles[0][0];
    std::string role_arn = roles[0][1];

    aws::get_creds(unescaped, principal_arn, role_arn);

    curl_global_cleanup();
    return 0;
}
