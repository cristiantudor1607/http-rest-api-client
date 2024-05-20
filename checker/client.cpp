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

int main() {
    int sockfd = -1;
    bool stop = false;
    char *accumulator;
    string input, request, response;
    string username, password;

    for (;;) {
        getline(cin, input);
        int opcode = parse_input(input);
        switch (opcode) {
            case REGISTER:
                prompt_credentials(username, password);
                request = generate_register_request(username, password);
                if (open_connection(&sockfd) < 0) {
                    stop = true;
                    break;
                }

                cout << "Sending request...\n";
                send_to_server(sockfd, request.data());
                cout << "Response:\n";
                accumulator = receive_from_server(sockfd);
                cout << accumulator;
                close_connection(sockfd);

                break;
            case LOGIN:
                prompt_credentials(username, password);
                request = generate_login_request(username, password);
                if (open_connection(&sockfd) < 0) {
                    stop = true;
                    break;
                }

                cout << "Sending request...\n";
                send_to_server(sockfd, request.data());
                cout << "Response:\n";
                accumulator = receive_from_server(sockfd);
                cout << accumulator;
                close_connection(sockfd);
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