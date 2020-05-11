#include <iostream>
#include <regex>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

#include "base64.hpp"
#include "okta.hpp"
#include "unescape.hpp"
#include "xml.hpp"
#include "zlib.hpp"

using json = nlohmann::json;

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

int main(void)
{

    curl_global_init(CURL_GLOBAL_ALL);

    std::string org = "";
    std::string username = "";
    std::string password = "";

    org_prompt(org);
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

    curl_global_cleanup();
    return 0;
}
