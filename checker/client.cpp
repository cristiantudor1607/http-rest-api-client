#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

#include <string>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "include/defines.hpp"
#include "include/http-utils.hpp"

#include "include/utils.hpp"

#define OPEN_CONN_FAIL (-2)
#define MEM_FAIL (-1)
#define LOGIN_FAIL 1
#define LOGIN_SUCCESS 2
#define REGISTER_FAIL 1
#define REGISTER_SUCCESS 2
#define OK 0

using namespace std;

int open_connection(int *sockfd) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        *sockfd = -1;
        fprintf(stderr, "[ERROR] socket() failed. Exiting...\n");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &(addr.sin_addr));

    if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        *sockfd = -1;
        fprintf(stderr, "[ERROR] connect() failed. Exiting...\n");
        close(fd);
        return - 1;
    }

    *sockfd = fd;
    return 0;
}

void close_connection(int sockfd) {
    close(sockfd);
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}

int do_register() {
    int sockfd = -1;
    char *acc = NULL;

    string request;
    string username, password;

    prompt_credentials(username, password);
    request = generate_register_request(username, password);
    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);
    if (!acc) {
        fprintf(stderr, "[ERROR] Memory fail at receiving data from server. Exiting...\n");
        return MEM_FAIL;
    }

    int ret;
    char *json_start = basic_extract_json_response(acc);
    if (json_start == NULL) {
        ret = REGISTER_SUCCESS;
        fprintf(stdout, "[SUCCESS] [201 Created] User %s registered.\n", username.c_str());
    } else {
        fprintf(stdout, "[ERROR] [400 Bad Request] %s\n", extract_error(json_start).c_str());
        ret = REGISTER_FAIL;
    }

    free(acc);
    return ret;
}

int do_login(string& token) {
    int sockfd = -1;
    char *acc = NULL;

    string request;
    string username, password;

    prompt_credentials(username, password);
    request = generate_login_request(username, password);
    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);
    if (!acc) {
        fprintf(stderr, "[ERROR] Memory fail at receiving data from server. Exiting...\n");
        return MEM_FAIL;
    }

    int ret;
    char *json_start = basic_extract_json_response(acc);
    if (json_start == NULL) {
        token = extract_connect_sid(acc);
        fprintf(stdout, "[SUCCESS] [200 OK] User %s logged in.\n", username.c_str());
        ret = LOGIN_SUCCESS;
    } else {
        token = "";
        fprintf(stdout, "[ERROR] [400 Bad Request] %s\n", extract_error(json_start).c_str());
        ret = LOGIN_FAIL;
    }

    free(acc);
    return ret;
}

int main() {
    bool stop = false;
    string input, token;
    int ret;

    for (;;) {
        getline(cin, input);
        int opcode = parse_input(input);
        switch (opcode) {
            case REGISTER:
                ret = do_register();
                if (ret < 0)
                    stop = true;

                break;
            case LOGIN:
                if (do_login(token) < 0)
                    stop = true;

                break;
            case ENTER_LIBRARY:
                cout << "ENTER LIBRARY\n";
                break;
            case GET_BOOKS:
                cout << "GET MULTIPLE BOOKS\n";
                break;
            case GET_BOOK:
                cout << "GET BOOK\n";
                break;
            case ADD_BOOK:
                cout << "ADD BOOK\n";
                break;
            case DELETE_BOOK:
                cout << "DELETE BOOK\n";
                break;
            case LOGOUT:
                cout << "LOGOUT\n";
                break;
            case EXIT:
                cout << "EXIT\n";
                stop = true;
                break;
            default:
                cout << "[ERROR] Unrecognised command\n";
                break;
        }

        if (stop)
            break;
    }

    return 0;
}