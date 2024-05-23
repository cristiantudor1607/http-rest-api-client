#include "../include/routines.hpp"

static inline void display_memory_fail() {
    fprintf(stdout, "[ERROR] Memory fail at receiving data from server. "
                    "Exiting...\n");
}

static int parse_register_response(string& username, char *response) {
    string code = extract_http_status_code(response);
    char *json_start = basic_extract_json_response(response);
    if (json_start == NULL) {
        fprintf(stdout, "[SUCCESS] [%s] User %s registered.\n",
                code.c_str(), username.c_str());
        return SUCCESS;
    }

    fprintf(stdout, "[ERROR] [%s] %s\n", code.c_str(),
            extract_error(json_start).c_str());
    return FAIL;
}

static void parse_add_book_response(char *response) {
    string code = extract_http_status_code(response);
    if (code == "200 OK") {
        fprintf(stdout, "[SUCCESS] [200 OK] Book added to your library.\n");
        return;
    }

    char *json = basic_extract_json_response(response);
    fprintf(stdout, "[ERROR] [%s] %s\n", code.c_str(), extract_error(json).c_str());
}

static int parse_get_book_response(char *response) {
    string code = extract_http_status_code(response);

    char *json_start = basic_extract_json_response(response);
    if (code != "200 OK") {
        fprintf(stdout, "[ERROR] [%s] %s\n", code.c_str(),
                extract_error(json_start).c_str());

        return FAIL;
    }

    json j = json::parse(json_start);
    fprintf(stdout, "[SUCCESS] [200 OK] Book was found:\n");
    fprintf(stdout, "%s\n", j.dump(STD_INTEND).c_str());
    return SUCCESS;
}

static int parse_delete_book_response(char *response) {
    string code = extract_http_status_code(response);

    char *json_start = basic_extract_json_response(response);
    if (code != "200 OK") {
        fprintf(stdout, "[ERROR] [%s] %s\n", code.c_str(),
                extract_error(json_start).c_str());

        return FAIL;
    }

    fprintf(stdout, "[SUCCESS] [200 OK] Book was deleted from your library.\n");
    return SUCCESS;
}

static void print_books(string& user, char *response) {
    if (strstr(response, "200 OK") == NULL) {
        char *json_start = basic_extract_json_response(response);
        fprintf(stdout, "[ERROR] %s\n", extract_error(json_start).c_str());
        return;
    }

    if (strstr(response, "[]")) {
        fprintf(stdout, "[SUCCESS] [200 OK] %s's library is empty.\n", user.c_str());
        fprintf(stdout, "[]\n");
        return;
    }

    char *ptr = strstr(response, "[");
    json j = json::parse(ptr);
    fprintf(stdout, "[SUCCESS] %s's library:\n", user.c_str());
    fprintf(stdout, "%s\n", j.dump(STD_INTEND).c_str());
}

// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
static int check_credential(string& credential) {
    stringstream ss(credential);

    string word;
    getline(ss, word, ' ');
    credential = word;

    if (getline(ss, word, ' '))
        return FORMAT_WRONG;

    credential = word;
    return FORMAT_OK;
}

static int check_id(string& buff) {
    if (!has_only_digits(buff))
        return INVALID;

    int res;
    try {
        res = stoi(buff);
    } catch (exception e) {
        return INVALID;
    }

    return res < 0 ? INVALID : res;
}

static void prompt_credentials(string& username, string& password) {
    fprintf(stdout, "username=");
    getline(cin, username);
    fprintf(stdout, "password=");
    getline(cin, password);

    remove_leading_whitespaces(username);
    remove_trailing_whitespaces(username);

    remove_leading_whitespaces(password);
    remove_trailing_whitespaces(password);

    if (check_credential(username) < 0 || check_credential(password) < 0) {
        cout << "[ERROR] Credentials should not contain spaces.\n";
        username = password = "";
    }
}

static int prompt_id() {
    string buff;

    fprintf(stdout, "id=");
    getline(cin, buff);

    remove_leading_whitespaces(buff);
    remove_trailing_whitespaces(buff);

    int ret = check_id(buff);
    return ret < 0 ? -1 : ret;
}


int do_register(SessionData *data) {
    int sockfd = -1;
    char *acc = NULL;

    if (data->connected) {
        fprintf(stdout, "[ERROR] You're connected as %s. "
                        "You should logout before creating a new account.\n",
                        data->username.c_str());
        return FAIL;
    }

    string request;
    string username, password;

    prompt_credentials(username, password);
    if (username.empty() || password.empty()) {
        fprintf(stdout, "[ERROR] Empty credentials are not allowed.\n");
        return CREDENTIAL_FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    request = generate_register_request(username, password);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);
    if (!acc) {
        display_memory_fail();
        return MEM_FAIL;
    }

    int ret = parse_register_response(username, acc);
    free(acc);
    return ret;
}

int do_login(SessionData *data) {
    int sockfd = -1;
    char *acc = NULL;
    string username, password;

    if (data->connected) {
        fprintf(stdout, "[ERROR] You're already connected as %s. "
                        "Logout and try again.\n", data->username.c_str());
        return FAIL;
    }

    prompt_credentials(username, password);
    if (username.empty() || password.empty()) {
        fprintf(stdout, "[ERROR] Empty credentials are not allowed.\n");
        return CREDENTIAL_FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_login_request(username, password);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);
    if (!acc) {
        display_memory_fail();
        return MEM_FAIL;
    }

    string code = extract_http_status_code(acc);
    char *json_start = basic_extract_json_response(acc);
    if (json_start == NULL) {
        data->sid = extract_connect_sid(acc);
        data->jwt = "";
        data->username = username;
        data->connected = true;
        fprintf(stdout, "[SUCCESS] [%s] User %s logged in.\n",
                code.c_str(), username.c_str());
        free(acc);
        return SUCCESS;
    }

    data->reset();
    fprintf(stdout, "[ERROR] [%s] %s\n", code.c_str(),
            extract_error(json_start).c_str());

    free(acc);
    return FAIL;
}

int do_enter_library(SessionData *data) {
    int sockfd = -1;
    char *acc;

    if (!data->connected) {
        fprintf(stdout, "[ERROR] User not logged in.\n");
        return FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_access_request(data->sid);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        display_memory_fail();
        return MEM_FAIL;
    }

    string code = extract_http_status_code(acc);
    char *json_start = basic_extract_json_response(acc);
    if (code != "200 OK") {
        fprintf(stdout, "[ERROR] [%s] %s\n",
                code.c_str(), extract_error(json_start).c_str());

        free(acc);
        return FAIL;
    }

    data->jwt = extract_token(json_start);
    data->access = true;
    fprintf(stdout, "[SUCCESS] [200 OK] The user %s has acquired "
                    "access to library.\n", data->username.c_str());

    free(acc);
    return SUCCESS;
}

int do_get_books(SessionData *data) {
    int sockfd = -1;
    char *acc;

    if (!data->connected) {
        fprintf(stdout, "[ERROR] User not logged in.\n");
        return FAIL;
    }

    if (!data->access) {
        fprintf(stdout, "[ERROR] User %s doesn't have access "
                        "to library.\n", data->username.c_str());
        return FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_get_books_request(data->jwt);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        display_memory_fail();
        return MEM_FAIL;
    }

    print_books(data->username, acc);
    free(acc);
    return SUCCESS;
}

int do_add_book(SessionData *data) {
    if (!data->connected) {
        fprintf(stdout, "[ERROR] User not logged in.\n");
        return FAIL;
    }

    if (!data->access) {
        fprintf(stdout, "[ERROR] User %s doesn't have access "
                        "to library.\n", data->username.c_str());
        return FAIL;
    }

    Book book;
    book.read();
    book.clean_text();
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
    string request = generate_add_book_request(obj, data->jwt);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        display_memory_fail();
        return MEM_FAIL;
    }

    parse_add_book_response(acc);
    free(acc);
    return SUCCESS;
}

int do_logout(SessionData *data) {
    int sockfd = -1;
    char *acc;

    if (!data->connected) {
        fprintf(stdout, "[ERROR] User not logged in.\n");
        return FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_logout_request(data->sid);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        display_memory_fail();
        return MEM_FAIL;
    }

    string code = extract_http_status_code(acc);
    if (code == "200 OK") {
        fprintf(stdout, "[SUCCESS] [200 OK] User %s logged out.\n", data->username.c_str());
        free(acc);
        data->reset();
        return SUCCESS;
    }

    // TODO: Add a reset here
    char *json = basic_extract_json_response(acc);
    fprintf(stdout, "[ERROR] [%s] %s\n", code.c_str(), extract_error(json).c_str());
    free(acc);
    return SUCCESS;
}

int do_get_book(SessionData *data) {
    int sockfd;
    char *acc;

    if (!data->connected) {
        fprintf(stdout, "[ERROR] User not logged in.\n");
        return FAIL;
    }

    if (!data->access) {
        fprintf(stdout, "[ERROR] User %s doesn't have access "
                        "to library.\n", data->username.c_str());
        return FAIL;
    }

    int id;
    if ((id = prompt_id()) < 0) {
        fprintf(stdout, "[ERROR] The id is not a number.\n");
        return FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_get_book_request(data->jwt, id);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    if (!acc) {
        display_memory_fail();
        return MEM_FAIL;
    }

    int ret = parse_get_book_response(acc);

    free(acc);
    return ret;
}

int do_delete_book(SessionData *data) {
    int sockfd;
    char *acc;

    if (!data->connected) {
        fprintf(stdout, "[ERROR] User not logged in.\n");
        return FAIL;
    }

    if (!data->access) {
        fprintf(stdout, "[ERROR] User %s doesn't have access "
                        "to library.\n", data->username.c_str());
        return FAIL;
    }

    int id;
    if ((id = prompt_id()) < 0) {
        fprintf(stdout, "[ERROR] The id is not a number.\n");
        return FAIL;
    }

    if (open_connection(&sockfd) < 0)
        return OPEN_CONN_FAIL;

    string request = generate_delete_book_request(data->jwt, id);
    send_to_server(sockfd, request.data());
    acc = receive_from_server(sockfd);
    close_connection(sockfd);

    parse_delete_book_response(acc);
    free(acc);
    return SUCCESS;
}
