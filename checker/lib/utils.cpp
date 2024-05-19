#include "../include/utils.hpp"

void prompt_credentials(string& username, string& password) {
    fprintf(stdout, "username=");
    getline(cin, username);
    fprintf(stdout, "password=");
    getline(cin, password);
}