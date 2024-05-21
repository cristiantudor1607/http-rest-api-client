#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <iostream>
#include <sstream>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "buffer.h"
#ifdef __cplusplus
}
#endif

#include "defines.hpp"
#include "json.hpp"

#define FORMAT_WRONG (-1)
#define FORMAT_OK 0

using namespace std;


void prompt_credentials(string& username, string& password);
int parse_input(string& input);

int send_to_server(int sockfd, const char *message);
char *receive_from_server(int sockfd);
char *basic_extract_json_response(char *str);

#endif //_UTILS_HPP
