#ifndef _ROUTINES_HPP
#define _ROUTINES_HPP

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>

#include "../include/defines.hpp"
#include "../include/book.hpp"
#include "../include/http-utils.hpp"
#include "../include/utils.hpp"

#define OPEN_CONN_FAIL (-2)
#define MEM_FAIL (-1)
#define FAIL 1
#define SUCCESS 2
#define CREDENTIAL_FAIL 3
#define OK 0

#define INVALID -10

using namespace std;

int do_register(SessionData *data);
int do_login(SessionData *data);
int do_enter_library(SessionData *data);
int do_get_books(SessionData *data);
int do_add_book(SessionData *data);
int do_logout(SessionData *data);
int do_get_book(SessionData *data);
int do_delete_book(SessionData *data);

#endif //_ROUTINES_HPP
