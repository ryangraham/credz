#include <curl/curl.h>

#include <string>

namespace curl {

static size_t write_callback(void *buffer, size_t size, size_t count,
                             void *string) {
  ((std::string *)string)->append((char *)buffer, size * count);
  return size * count;
}

int get(const std::string &url, std::string &buffer,
        const std::string &session_id) {
  CURL *curl;
  CURLcode res;
  int ret = 0;

  std::string cookie = "Cookie: sid=" + session_id;

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");
  headers = curl_slist_append(headers, cookie.c_str());

  curl = curl_easy_init();
  if (!curl) return -1;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
  res = curl_easy_perform(curl);

  if (res == CURLE_HTTP_RETURNED_ERROR) ret = -1;

  curl_easy_cleanup(curl);

  return ret;
}

int post(const std::string &url, const std::string &payload,
         std::string &buffer) {
  CURL *curl;
  CURLcode res;
  int ret = 0;

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  curl = curl_easy_init();
  if (!curl) return -1;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
  res = curl_easy_perform(curl);

  if (res == CURLE_HTTP_RETURNED_ERROR) ret = -1;

  curl_easy_cleanup(curl);

  return ret;
}

}  // namespace curl
