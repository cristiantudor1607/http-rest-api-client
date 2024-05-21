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
#include "include/book.hpp"
#include "include/http-utils.hpp"
#include "include/utils.hpp"

#define OPEN_CONN_FAIL (-2)
#define MEM_FAIL (-1)
#define FAIL 1
#define SUCCESS 2
#define CREDENTIAL_FAIL 3
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
    if (username.empty() || password.empty())
        return CREDENTIAL_FAIL;

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
        ret = SUCCESS;
        fprintf(stdout, "[SUCCESS] [201 Created] User %s registered.\n", username.c_str());
    } else {
        fprintf(stdout, "[ERROR] [400 Bad Request] %s\n", extract_error(json_start).c_str());
        ret = FAIL;
    }

    free(acc);
    return ret;
}

int do_login(string& user, string& sid) {
    user = "";

    int sockfd = -1;
    char *acc = NULL;

    string request;
    string username, password;

    prompt_credentials(username, password);
    if (username.empty() || password.empty())
        return CREDENTIAL_FAIL;

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
        sid = extract_connect_sid(acc);
        user = username;
        fprintf(stdout, "[SUCCESS] [200 OK] User %s logged in.\n", user.c_str());
        ret = SUCCESS;
    } else {
        sid = "";
        fprintf(stdout, "[ERROR] [400 Bad Request] %s\n", extract_error(json_start).c_str());
        ret = FAIL;
    }

    free(acc);
    return ret;
}

int do_enter_library(string& user, string& sid, string& jwt) {
    int sockfd = -1;
    char *acc;

    if (sid.empty()) {
        fprintf(stdout, "[ERROR] User not logged in.\n");
        jwt = "";
        return FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_access_request(sid);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        fprintf(stderr, "[ERROR] Memory fail at receiving data from server. Exiting...\n");
        return MEM_FAIL;
    }

    char *json_start = basic_extract_json_response(acc);
    jwt = extract_token(json_start);

    fprintf(stdout, "[SUCCESS] [200 OK] The user %s has acquired access"
                    "to library.\n",
            user.c_str());
    free(acc);

    return SUCCESS;
}

int do_get_books(string& user, string& jwt) {
    int sockfd = -1;
    char *acc;

    if (jwt.empty()) {
        fprintf(stdout, "[ERROR] User %s doesn't have access to library.\n", user.c_str());
        return FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_get_books_request(jwt);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        fprintf(stderr, "[ERROR] Memory fail at receiving data from server. Exiting...\n");
        return MEM_FAIL;
    }

    // TODO: Trebuie printate cartile
    cout << acc << endl;
    free(acc);
    return SUCCESS;
}

int do_add_book(string& user, string& sid, string& jwt) {
    if (sid.empty()) {
        fprintf(stdout, "[ERROR] User not logged in.\n");
        return FAIL;
    }

    if (jwt.empty()) {
        fprintf(stdout, "[ERROR] User %s doesn't have access to library.\n", user.c_str());
        return FAIL;
    }

    Book book;
    book.read();
    switch (book.validate()) {
        case PAGE_COUNT_WRONG:
            fprintf(stdout, "[ERROR] The given page count isn't a number.\n");
            return FAIL;
        case EMPTY_FIELDS:
            fprintf(stdout, "[ERROR] Empty strings are not allowed.\n");
            return FAIL;
        default:
            break;
    }

    int sockfd;
    char *acc;
    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    return SUCCESS;
}

int main() {
    bool stop = false;
    string input, sid, jwt;
    string username;

    for (;;) {
        getline(cin, input);
        int opcode = parse_input(input);
        switch (opcode) {
            case REGISTER:
                if (do_register() < 0)
                    stop = true;
                break;
            case LOGIN:
                if (do_login(username, sid) < 0)
                    stop = true;
                break;
            case ENTER_LIBRARY:
                if (do_enter_library(username, sid, jwt) < 0)
                    stop = true;

                break;
            case GET_BOOKS:
                if (do_get_books(username, jwt) < 0)
                    stop = true;

                break;
            case GET_BOOK:
                cout << "GET BOOK\n";
                break;
            case ADD_BOOK:
                if (do_add_book(username, sid, jwt) < 0)
                    stop = true;

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
                fprintf(stdout, "[ERROR] Unknown command.\n");
                break;
        }

        if (stop)
            break;
    }

    return 0;
}