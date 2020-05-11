#include <string>
#include <iostream>
#include <boost/regex.hpp>
#include <curl/curl.h>

/*
 * URL decode a string
 * 
 * Example: "%3D" becomes "="
 * 
 * Note: unused for now
 */
std::string url_decode(const std::string &encoded)
{
    CURL *curl = curl_easy_init();
    int output_len;
    char *curl_result = curl_easy_unescape(curl, encoded.c_str(), encoded.length(), &output_len);
    std::string res(curl_result, curl_result + output_len);
    curl_free(curl_result);
    curl_easy_cleanup(curl);
    return res;
}

/*
 * Unescape HTML entities
 * 
 * Example: "&#x3D;" becomes "="
 * 
 * https://en.wikipedia.org/wiki/Numeric_character_reference
 */
std::string unescape(const std::string &input)
{
    std::string output;
    boost::regex re("&#x([a-zA-Z0-9][a-zA-Z0-9]);");
    const int subs[] = {-1, 1};
    boost::sregex_token_iterator end;

    boost::sregex_token_iterator token(input.begin(), input.end(), re, subs);

    for (bool is_ncr = false; token != end; ++token, is_ncr = !is_ncr)
    {
        if (is_ncr)
        {
            unsigned int d = std::stoul("0x" + *token, nullptr, 16);
            output += static_cast<char>(d);
        }
        else
            output += *token;
    }

    return output;
}