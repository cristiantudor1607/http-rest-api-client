#ifndef _CLIENT_BOOK_HPP
#define _CLIENT_BOOK_HPP

#include <iostream>
#include <string>

#include "json.hpp"

#define PAGE_COUNT_WRONG (-1)
#define EMPTY_FIELDS (-2)
#define INPUT_OK 0

using namespace std;
using json = nlohmann::json;

class Book {
private:
    string title;
    string author;
    string genre;
    size_t page_count;
    string publisher;

    string input_page_count;

public:
    Book();
    void read();
    int validate();
    string pack_to_json();
    void print_book();
};

#endif //_CLIENT_BOOK_HPP
