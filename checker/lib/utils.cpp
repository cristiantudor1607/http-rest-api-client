#include "../include/utils.hpp"

#define BUFLEN 4096
#define LINELEN 1000

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void remove_trailing_whitespaces(string& s) {
    char *begin = &s[0];

    char *ptr = &s[s.size() - 1];
    while (*ptr == ' ' && ptr >= &s[0]) {
        *ptr = '\0';
        ptr--;
    }

    s = string(begin);
}

int parse_input(string& input) {
    remove_trailing_whitespaces(input);

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

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}

bool has_only_digits(string& s) {
    for (size_t i = 0; i < s.size(); i++) {
        if (!isdigit(s[i]))
            return false;
    }

    return true;
}

SessionData::SessionData() {
    this->status = 0;
    reset();
}

void SessionData::reset() {
    this->username = "";
    this->sid = "";
    this->jwt = "";
    this->connected = false;
    this->access = false;
}

