#include "../include/utils.hpp"

void prompt_credentials(string& username, string& password) {
    fprintf(stdout, "username=");
    getline(cin, username);
    fprintf(stdout, "password=");
    getline(cin, password);
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