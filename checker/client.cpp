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

using namespace std;

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
                if (do_register(sessionData) < 0)
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
                if (do_get_book(sessionData) < 0)
                    stop = true;
                break;
            case ADD_BOOK:
                if (do_add_book(sessionData) < 0)
                    stop = true;
                break;
            case DELETE_BOOK:
                if (do_delete_book(sessionData) < 0)
                    stop = true;
                break;
            case LOGOUT:
                if (do_logout(sessionData) < 0)
                    stop = true;
                break;
            case EXIT:
                stop = true;
                break;
            default:
                fprintf(stdout, "[ERROR] Query not supported.\n");
                break;
        }

        if (stop)
            break;
    }

    delete sessionData;
    return 0;
}