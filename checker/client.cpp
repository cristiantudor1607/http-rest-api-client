#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

#include <string>

#include "include/defines.hpp"
#include "include/book.hpp"
#include "include/http-utils.hpp"
#include "include/utils.hpp"
#include "include/routines.hpp"

//#define OPEN_CONN_FAIL (-2)
//#define MEM_FAIL (-1)
//#define FAIL 1
//#define SUCCESS 2
//#define CREDENTIAL_FAIL 3
//#define OK 0

using namespace std;

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

    string obj = book.pack_to_json();
    string request = generate_add_book_request(obj, jwt);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        fprintf(stderr, "[ERROR] Memory fail at receiving data from server. Exiting...\n");
        return MEM_FAIL;
    }

    parse_add_book_response(acc);
    free(acc);
    return SUCCESS;
}

int do_logout(string& sid) {
    int sockfd = -1;
    char *acc;

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_logout_request(sid);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        fprintf(stderr, "[ERROR] Memory fail at receiving data from server. Exiting...\n");
        return MEM_FAIL;
    }

    cout << acc << endl;
    free(acc);
    return SUCCESS;
}

int main() {
    auto *sessionData = new (nothrow) SessionData();
    if (!sessionData) {
        fprintf(stderr, "[ERROR] Memory fail at initializing the client. "
                        "Exiting...\n");
        return EXIT_FAILURE;
    }

    bool stop = false;
    string input;
    string username, sid, jwt;

    for (;;) {
        getline(cin, input);
        int opcode = parse_input(input);
        switch (opcode) {
            case REGISTER:
                if (do_register() < 0)
                    stop = true;
                break;
            case LOGIN:
                if (do_login(sessionData) < 0)
                    stop = true;
                break;
            case ENTER_LIBRARY:
                if (do_enter_library(sessionData) < 0)
                    stop = true;

                break;
            case GET_BOOKS:
                if (do_get_books(sessionData) < 0)
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
                if (do_logout(sid) < 0)
                    stop = true;
                break;
            case EXIT:
                stop = true;
                break;
            default:
                fprintf(stdout, "[ERROR] Unknown command.\n");
                break;
        }

        if (stop)
            break;
    }

    return 0;
}