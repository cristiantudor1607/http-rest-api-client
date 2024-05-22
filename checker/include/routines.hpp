#ifndef _ROUTINES_HPP
#define _ROUTINES_HPP

#include <iostream>
#include <cstdlib>
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

using namespace std;

int do_register();
int do_login(SessionData *data);
int do_enter_library(SessionData *data);
int do_get_books(SessionData *data);

#endif //_ROUTINES_HPP
