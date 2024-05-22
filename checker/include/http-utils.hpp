//
// Created by Cristian-Andrei Tudor on 18.05.2024.
//

#ifndef HTTP_UTILS_HPP
#define HTTP_UTILS_HPP

#include <iostream>
#include <sstream>


#include "defines.hpp"
#include "utils.hpp"
#include "json.hpp"

#define STD_INTEND 4

using namespace std;
using json = nlohmann::json;

string extract_http_status_code(char *response);
string extract_connect_sid(char *response);
string extract_token(char *json_start);
string extract_error(char *json_start);


string generate_register_request(string& username, string& password);
string generate_login_request(string& username, string& password);
string generate_access_request(string& sid);
string generate_get_books_request(string& jwt);
string generate_add_book_request(string& obj, string& jwt);
string generate_get_book_request(string& jwt, int id);
string generate_logout_request(string& sid);

#endif // HTTP_UTILS_HPP
