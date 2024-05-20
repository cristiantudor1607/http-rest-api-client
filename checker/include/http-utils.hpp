//
// Created by Cristian-Andrei Tudor on 18.05.2024.
//

#ifndef HTTP_UTILS_HPP
#define HTTP_UTILS_HPP

#include <iostream>
#include "defines.hpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

string extract_connect_sid(char *response);
string extract_error(char *json_start);

string generate_register_request(string& username, string& password);
string generate_login_request(string& username, string& password);

#endif //HTTP_UTILS_HPP
