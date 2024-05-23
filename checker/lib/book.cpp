#include "../include/book.hpp"

Book::Book() {
    title = "";
    author = "";
    genre = "";
    page_count = 0;
    publisher = "";
}

void Book::read() {
    fprintf(stdout, "title=");
    getline(cin, title);

    fprintf(stdout, "author=");
    getline(cin, author);

    fprintf(stdout, "genre=");
    getline(cin, genre);

    fprintf(stdout, "publisher=");
    getline(cin, publisher);

    fprintf(stdout,  "page_count=");
    getline(cin, input_page_count);
}

void Book::clean_text() {
    remove_leading_whitespaces(title);
    remove_leading_whitespaces(author);
    remove_leading_whitespaces(genre);
    remove_leading_whitespaces(publisher);
    remove_leading_whitespaces(input_page_count);

    remove_trailing_whitespaces(title);
    remove_trailing_whitespaces(author);
    remove_trailing_whitespaces(genre);
    remove_trailing_whitespaces(publisher);
    remove_trailing_whitespaces(input_page_count);
}

int Book::validate() {
    if (title.empty() || author.empty() ||
        genre.empty() || publisher.empty() ||
        input_page_count.empty())
        return EMPTY_FIELDS;

    // TODO: Check for numbers in author
    // TODO: Check for numbers in genre

    if (!has_only_digits(input_page_count))
        return PAGE_COUNT_WRONG;

    size_t aux;
    try {
        aux = stoul(input_page_count);
        page_count = aux;
    } catch (exception& e) {
        return PAGE_COUNT_WRONG;
    }

    return INPUT_OK;
}

string Book::pack_to_json() {
    json j;
    j["title"] = title;
    j["author"] = author;
    j["genre"] = genre;
    j["publisher"] = publisher;
    j["page_count"] = page_count;

    return j.dump();
}

void Book::print_book() {
    fprintf(stdout, "title=%s\n", title.c_str());
    fprintf(stdout, "author=%s\n", author.c_str());
    fprintf(stdout, "genre=%s\n", genre.c_str());
    fprintf(stdout, "publisher=%s\n", publisher.c_str());
    fprintf(stdout, "page_count=%s\n", input_page_count.c_str());
}

