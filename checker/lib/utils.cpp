#include "../include/utils.hpp"

#define BUFLEN 4096
#define LINELEN 1000

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

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

void prompt_credentials(string& username, string& password) {
    fprintf(stdout, "username=");
    getline(cin, username);
    fprintf(stdout, "password=");
    getline(cin, password);

    if (check_credential(username) < 0 || check_credential(password) < 0) {
        cout << "[ERROR] Credentials should not contain spaces.\n";
        username = password = "";
    }
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

int send_to_server(int sockfd, const char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            fprintf(stderr, "[ERROR] Failed to write request to socket. Exiting...\n");
            return -1;
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);

    return sent;
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            fprintf(stderr, "[ERROR] Failed to read response from socket. Exiting...\n");
            buffer_destroy(&buffer);
            return NULL;
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);

        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;

            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);

            if (content_length_start < 0) {
                continue;
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;

    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            fprintf(stderr, "[ERROR] Failed to read response from socket. Exiting...\n");
            buffer_destroy(&buffer);
            return NULL;
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}
