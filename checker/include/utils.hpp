#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <iostream>
#include <sstream>
#include <cctype>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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

int open_connection(int *sockfd);
void close_connection(int sockfd);

int parse_input(string& input);

int send_to_server(int sockfd, const char *message);
char *receive_from_server(int sockfd);
char *basic_extract_json_response(char *str);

bool has_only_digits(string& s);

class SessionData {
public:
    string username;
    string jwt, sid;
    bool connected;
    bool access;

    void reset();
};

#endif //_UTILS_HPP
