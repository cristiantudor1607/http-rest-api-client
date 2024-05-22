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

int do_register() {
    int sockfd = -1;
    char *acc = NULL;

    string request;
    string username, password;

    prompt_credentials(username, password);
    if (username.empty() || password.empty())
        return CREDENTIAL_FAIL;

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
    if (username.empty() || password.empty())
        return CREDENTIAL_FAIL;

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
