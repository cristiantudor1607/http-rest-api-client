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
#include "include/utils.hpp"

using namespace std;

void init_client(int *sockfd) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        *sockfd = -1;
        fprintf(stderr, "[ERROR] socket() failed. Exiting...\n");
        return;
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
        return;
    }

    *sockfd = fd;
}

int parse_input(string& input) {
    // TODO: Add pre-processing

    if (input == "register")
        return REGISTER;

    if (input == "login")
        return LOGIN;

    if (input == "enter_library")
        return ENTER_LIBRARY;

    if (input == "get_books")
        return GET_BOOKS;

    if (input == "get_book")
        return GET_BOOK;

    if (input == "add_book")
        return ADD_BOOK;

    if (input == "delete_book")
        return DELETE_BOOK;

    if (input == "logout")
        return LOGOUT;

    if (input == "exit")
        return EXIT;

    return -1;
}


int main() {
    int sockfd = -1;
    init_client(&sockfd);
    if (sockfd < 0)
        return 0;

    bool stop = false;
    string input;
    string username, password;
    for (;;) {
        getline(cin, input);
        int opcode = parse_input(input);
        switch (opcode) {
            case REGISTER:
                cout << "REGISTER\n";
                prompt_credentials(username, password);

                break;
            case LOGIN:
                cout << "LOGIN\n";
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

    close(sockfd);
    return 0;
}