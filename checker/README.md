## <font color="3C5B6F"> REST-API HTTP Client </font>

### <font color="3C5B6F"> How to use </font>
➢ First, if you don't have an account, you should make it using <font color="948979">**register**</font> command.
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

➢ Otherwise, you'll get an error.
> **!** **There are rules:**
> * 


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


### License
**Copyright &copy; May 2024, Cristian-Andrei Tudor 321CAa. Released under the PCom Team License.**