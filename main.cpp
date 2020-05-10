#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static size_t write_callback(void *buffer, size_t size, size_t count, void *string)
{
    ((std::string *)string)->append((char *)buffer, size * count);
    return size * count;
}

int post(const std::string &url, const std::string &payload, std::string &buffer)
{
    CURL *curl;
    CURLcode res;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");

    curl = curl_easy_init();
    if (!curl)
        return -1;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    // TODO: check response code

    return 0;
}

json okta_auth(const std::string &username, const std::string &password, const std::string &org)
{
    json auth;
    auth["username"] = username;
    auth["password"] = password;
    std::string payload = auth.dump();
    std::string url = "https://" + org + ".okta.com/api/v1/authn";
    std::string buffer;

    post(url, payload, buffer);

    json response = json::parse(buffer);

    std::cout << response["status"] << std::endl;
    // TODO: Check status

    return response;
}

int password_prompt(std::string &password)
{
    struct termios tty;

    // Get current terminal settings
    if (tcgetattr(STDIN_FILENO, &tty) != 0)
        return -1;

    // Unset ECHO flag
    tty.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0)
        return -1;

    // Prompt for password
    std::cout << "Password: ";
    std::cin >> password;

    // Set ECHO flag
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);

    return 0;
}

void username_prompt(std::string &username)
{
    std::cout << "Username: ";
    std::cin >> username;
}

std::string wait_for_push(const std::string &next_url, const std::string &payload)
{
    std::string buffer;
    post(next_url, payload, buffer);
    json response = json::parse(buffer);
    if (response["status"] == "SUCCESS")
        return response["sessionToken"];

    // TODO: handle timeout and rejection in factorStatus

    return wait_for_push(next_url, payload);
}

std::string verify_push(const json &factor, const std::string &state_token)
{
    json body;
    body["stateToken"] = state_token;
    std::string payload = body.dump();
    std::string url = factor["_links"]["verify"]["href"];
    std::string buffer;

    std::cout << url << std::endl;
    std::cout << payload << std::endl;

    post(url, payload, buffer);

    json response = json::parse(buffer);
    std::cout << response["status"] << std::endl;

    std::string next_url = response["_links"]["next"]["href"];

    return wait_for_push(next_url, payload);
}

std::string verify_mfa(const json &factors, const std::string &state_token)
{
    for (auto &factor : factors)
        if (factor["factorType"] == "push")
            return verify_push(factor, state_token);

    // TODO: fix
    return NULL;
}

std::string get_session_id(const std::string &session_token, const std::string &org)
{
    json body;
    body["sessionToken"] = session_token;
    std::string payload = body.dump();
    std::string url = "https://" + org + ".okta.com/api/v1/sessions";
    std::string buffer;

    post(url, payload, buffer);
    json response = json::parse(buffer);

    return response["id"];
}

int main(void)
{
    std::string username = "";
    std::string password = "";

    username_prompt(username);
    password_prompt(password);
    std::cout << std::endl;

    // TODO: Prompt for org
    std::string org = "blvd";
    json response = okta_auth(username, password, org);

    std::string state_token = response["stateToken"];

    json factors = response["_embedded"]["factors"];

    std::string session_token = verify_mfa(factors, state_token);

    std::cout << session_token << std::endl;

    std::string session_id = get_session_id(session_token, org);
    std::cout << session_id << std::endl;

    // TODO: get apps

    // TODO: get saml assertion

    // std::cout
    //     << response.dump(4) << std::endl;

    return 0;
}
