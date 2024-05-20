#include "../include/http-utils.hpp"

static inline void append_to_request(string& request, string& line) {
    request = request + line + "\r\n";
}

static inline void end_header(string& header) {
    header = header + "\r\n";
}

static string pack_credentials_to_json(string& username, string& password) {
    json j;
    j["username"] = username;
    j["password"] = password;

    return j.dump();
}

static string generate_json_post_request(string& path, string& json_obj) {
    string request = "";
    string temp = "POST " + path + " HTTP/1.1";
    append_to_request(request, temp);

    temp = "Host: " + string(IP);
    append_to_request(request, temp);

    temp = "Content-Type: application/json";
    append_to_request(request, temp);

    temp = "Content-Length: " + to_string(json_obj.size());
    append_to_request(request, temp);

    end_header(request);

    request = request + json_obj;
    return request;
}

string extract_connect_sid(char *response) {
    char *cookies = strstr(response, "Set-Cookie: ");

    char *token = strstr(cookies, "connect.sid=");
    token = token + (sizeof("connect.sid=") - 1);

    char *end = strstr(token, ";");

    return string(token, end - token);
}

string extract_token(char *json_start) {
    json j = json::parse(json_start);
    string s = j["token"];

    return s;
}

string extract_error(char *json_start) {
    json j = json::parse(json_start);
    string s = j["error"];

    return s;
}

string generate_register_request(string& username, string& password) {
    string obj = pack_credentials_to_json(username, password);
    string register_path = "/api/v1/tema/auth/register";

    return generate_json_post_request(register_path, obj);
}

string generate_login_request(string& username, string& password) {
    string obj = pack_credentials_to_json(username, password);

    string login_path = "/api/v1/tema/auth/login";
    return generate_json_post_request(login_path, obj);
}

string generate_access_request(string& sid) {
    string request = "";

    string temp = "GET /api/v1/tema/library/access HTTP/1.1";
    append_to_request(request, temp);

    temp = "Host: " + string(IP);
    append_to_request(request, temp);

    temp = "Cookie: connect.sid=" + sid + "; Path=/; HttpOnly";
    append_to_request(request, temp);

    end_header(request);
    return request;
}

string generate_get_books_request(string& jwt) {
    string request = "";

    string temp = "GET /api/v1/tema/library/books HTTP/1.1";
    append_to_request(request, temp);

    temp = "Host: " + string(IP);
    append_to_request(request, temp);

    temp = "Authorization: Bearer " + jwt;
    append_to_request(request, temp);

    end_header(request);
    return request;
}

