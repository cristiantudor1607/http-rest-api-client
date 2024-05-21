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

static string generate_json_post_request(string& path, string& json_obj,
                                         string& jwt, bool auth) {
    string request = "";
    string temp = "POST " + path + " HTTP/1.1";
    append_to_request(request, temp);

    temp = "Host: " + string(IP);
    append_to_request(request, temp);

    if (auth) {
        temp = "Authorization: Bearer " + jwt;
        append_to_request(request, temp);
    }

    temp = "Content-Type: application/json";
    append_to_request(request, temp);

    temp = "Content-Length: " + to_string(json_obj.size());
    append_to_request(request, temp);

    end_header(request);

    request = request + json_obj;
    return request;
}

string extract_http_status_code(char *response) {
    char *ptr = response + (sizeof("HTTP/1.1 ") - 1);

    char *end = strstr(response, "\r\n");
    *end = '\0';
    string ret = string(ptr);
    *end = '\r';

    return ret;
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

void print_books(string& user, char *response) {
    if (strstr(response, "200 OK") == NULL) {
        char *json_start = basic_extract_json_response(response);
        fprintf(stdout, "[ERROR] %s\n", extract_error(json_start).c_str());
        return;
    }

    if (strstr(response, "[]")) {
        fprintf(stdout, "[SUCCESS] [200 OK] %s's library is empty.\n", user.c_str());
        fprintf(stdout, "[]\n");
        return;
    }

    char *ptr = strstr(response, "[");
    json j = json::parse(ptr);
    fprintf(stdout, "[SUCCESS] %s's library:\n", user.c_str());
    fprintf(stdout, "%s\n", j.dump(STD_INTEND).c_str());
}

void parse_add_book_response(char *response) {
    string code = extract_http_status_code(response);
    if (code == "200 OK") {
        fprintf(stdout, "[SUCCESS] [200 OK] Book added to the library.\n");
        return;
    }

    char *json = basic_extract_json_response(response);
    fprintf(stdout, "[ERROR] [%s] %s\n", code.c_str(), extract_error(json).c_str());
}

string generate_register_request(string& username, string& password) {
    string obj = pack_credentials_to_json(username, password);
    string register_path = "/api/v1/tema/auth/register";
    string fake = "";

    return generate_json_post_request(register_path, obj, fake, false);
}

string generate_login_request(string& username, string& password) {
    string obj = pack_credentials_to_json(username, password);
    string login_path = "/api/v1/tema/auth/login";
    string fake = "";

    return generate_json_post_request(login_path, obj, fake, false);
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

string generate_add_book_request(string& obj, string& jwt) {
    string add_book_path = "/api/v1/tema/library/books";

    return generate_json_post_request(add_book_path, obj, jwt, true);
}

string generate_logout_request(string& sid) {
    string request = "";

    string temp = "GET /api/v1/tema/auth/logout";
    append_to_request(request, temp);

    temp = "Host: " + string(IP);
    append_to_request(request, temp);

    temp = "Cookie: connect.sid=" + sid;
    append_to_request(request, temp);

    end_header(request);
    return request;
}

