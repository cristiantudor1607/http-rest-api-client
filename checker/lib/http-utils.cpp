#include "../include/http-utils.hpp"

static inline void append_to_request(string& request, string& line) {
    request = request + line + "\r\n";
}

static inline void end_header(string& header) {
    header = header + "\r\n";
}

string pack_credentials_to_json(string& username, string& password) {
    json j;
    j["username"] = username;
    j["password"] = password;

    return j.dump(JSON_INTEND);
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

string generate_register_request(string& username, string& password) {
//    string request = "";
//    string temp;
//    temp = "POST /api/v1/tema/auth/register HTTP/1.1";
//
//    append_to_request(request, temp);
//
//    temp = "Host: " + string(IP);
//    append_to_request(request, temp);
//
//    temp = "Content-Type: application/json";
//    append_to_request(request, temp);
//
//    string obj = pack_credentials_to_json(username, password);
//    temp = "Content-Length: " + to_string(obj.size());
//    append_to_request(request, temp);
//    end_header(request);
//
//    request = request + obj;
    string obj = pack_credentials_to_json(username, password);
    string register_path = "/api/v1/tema/auth/register";

    return generate_json_post_request(register_path, obj);
}

string generate_login_request(string& username, string& password) {
    string obj = pack_credentials_to_json(username, password);

    string login_path = "/api/v1/tema/auth/login";
    return generate_json_post_request(login_path, obj);
}
