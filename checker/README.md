## <font color="3C5B6F"> REST-API HTTP Client </font>

### <font color="3C5B6F"> How to use </font>
➢ First, if you don't have an account, you should make one using <font color="948979">**register**</font> command.
```
> register
< username=
> $your-username$
< password=
> $your-password$
```
➢ If the username you provided isn't taken, you should get the message:
> <font color="41B06E">[SUCCESS] [201 Created] User $your-username$ registered. </font>
>

➢ Otherwise, you'll get an error.
> All errors are reported by <font color="E72929">[ERROR]</font> keyword, followed by a specific message describing
> the error.


➢ After register, you are not logged in.

➢ Now, login to your account by using <font color="948979">**login**</font> command. It will show the same prompt for
username and password as register.

➢ If the account exists, you should get:
> <font color="41B06E">[SUCCESS] [200 OK] User $your-username$ logged in. </font>

➢ Otherwise, you'll get an error. <br>

➢ Now, to be able to do changes or see your library, you should access your library by typing:
```
> enter_library
```
➢ It shouldn't throw an error if you logged in previously, but you'll be notified for sure if something went
wrong. Otherwise, it'll prompt the following message:
> <font color="41B06E">[SUCCESS] [200 OK] The user $username$ has acquired access to library.</font>

> **!** **There are some intuitive rules:**
> * Once you're logged in, you can't register a new account or connect to other account, until you disconnect (logout).
> * If you want to access your library (get books, add books, delete books), you have to **enter library** first.
> * You can exit any time, but you'll be disconnected, if you were connected.


### <font color="3C5B6F"> Project Structure </font>
* <font color="948979">**client.cpp**</font> &rarr; The entry point of the client. It contains the main() function.
* <font color="948979">**json.hpp**</font> &rarr; The Nlohmann JSON Parser header file.
* <font color="948979">**routines.cpp / routines.cpp**</font> &rarr;
    * The main() function is very short, it contains only a switch-case statement, to select which routine should be
executed based on the input provided, so there is no relevant code in client.cpp file. The client is implemented as an
abstract entity, which only calls routines.
    * routines.cpp contains the implementation of the routines.
    * For what is a routine, see [this](#routines) section.
* <font color="948979">**utils.cpp / utils.hpp**</font> &rarr; It contains general-purpose functions, like
**send_to_server** and **recieve_from_server**, taken from laboratory source code, functions for parsing user input
and the definition of **SessionData** class, whose only purpose is to encapsulate current user data and tokens.
* <font color="948979">**buffer.c / buffer.h**</font> &rarr; Laboratory source code. It is required by **send_to_server**
and **recieve_from_server** functions. The buffer.c code is compiled using gcc, and included in C++ code using
**extern "C"**. It contains the implementation of a **buffer** data structure. It provides APIs for working with
resizeable array of characters (it resembles with the **string** class from C++ standard library).
* <font color="948979">**defines.hpp**</font> &rarr; It contains only macros, used everywhere in the project
files.
* <font color="948979">**book.cpp / book.hpp**</font> &rarr;
  * The header file contains the definition of a small class named Book, and the C++ source file, the implementation of
this class.
  * Book class doesn't have too many methods:
    * The constructor, which creates an empty book.
    * read() &rarr; used to read a book from stdin. It takes the raw input, doesn't make any processing.
    * clean_text() &rarr; removes the leading and trailing whitespaces from the raw input.
    * validate() &rarr; validates the book fields.
    * pack_to_json() &rarr; makes a json book object from **this** instance. The caller should use validate() before
calling this method.
    * print_book() &rarr; used for debugging
    
      > **!** A book is empty if all it's fields are empty strings. <br> **!** A book is valid if none of its fields is
    empty and the page_count is a positive integer.

* <font color="948979">**http-utils.cpp / http-utils.hpp**</font> &rarr; It contains functions to generate HTTP Requests
and parse HTTP Responses.

### <a id="routines"></a> <font color="3C5B6F">Routines</font>
> **You can recognize a routine by its prefix. It always starts with do_$command_name$**.

➢ The routines take care of generating requests, sending requests, receiving responses and parse responses.
> There is always the same structure for a routine:
> * Make the sanity checks, based on the session data provided. It assures that the rules mentioned above are followed.
> <br> For example, it checks if the user is connected for all the command that require this, like enter_library, add_book,
> delete_book, get_books, get_book, etc. It also checks if the user has access to the library.
> * After the sanity checks, it generates the request and tries to send it. If some error occurred, it will return an
> error, defined as an integer, in **defines.hpp**.
> * It receives the response from the server and close the connection. If there are memory errors, it will drop an error, too.
> * It generates an output, based on the response provided and return either **SUCCESS** or **FAIL** (also defined in defines.hpp).

➢ The routines will modify the session data if needed. For example, when logging out, it will erase all the session data
fields.

### <font color="3C5B6F"> Observations </font>
> The **leading** and **trailing** whitespaces read from standard input are removed automatically. <br>
> It doesn't remove extra spaces between words.


### <font color="3C5B6F"> Improvements </font>
➢ It should use the HTTP Keep-Alive header, instead of opening and closing the connection each time a new request is sent. <br>
➢ More text validation could be done when reading infos of a book (but there were no restrictions specified). For example,
I would have imposed no numbers in the author name or genre, or the genre to be chosen from a predefined selection. <br>
➢ It should check is a book is already in library before adding it. In other words, it shouldn't allow duplicates. <br>
➢ There are some duplicate code in the parts involving generating requests and sanity checks, so the code should be refactored.

### <font color="3C5B6F"> Resources </font>
[1] : https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages &rarr; I read more about requests and responses, and I took
a look at how a request should look <br>
[2] : https://github.com/nlohmann/json &rarr; I used it to parse JSON contents of the responses. <br>
[3] : https://stackoverflow.com/questions/59457656/exception-handling-with-stoi-function &rarr; I read how to validate if
the input is really a number.<br>

### License
**Copyright &copy; May 2024, Cristian-Andrei Tudor 321CAa. Released under the PCom Team License.**