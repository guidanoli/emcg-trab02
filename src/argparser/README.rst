argparserlib
============

Parser for command line arguments of the types listed below.

Character flags
---------------

The keyword is a 1-byte long string, or, in other words, a
single charecter. If present, its value is true. e.g.:

  -h
    ...sets "h" to true
  -abc
    ...sets "a", "b" and "c" to true

Multi-character flags
---------------------

The keyword is a non-empty string. If present, its value is
equal to true. e.g.:

  --help
    ...sets "help" to true
  --v
    ...sets "v" to true
    ...is equivalent to -v

Valored flags
-------------

The keyword is a non-empty string. Following is an equal
sign (=), and the flag value, which is also a non-empty
string. e.g.:

  --verbose=y
    ...sets "verbose" to "y"
  --dir=mydir
    ...sets "dir" to "mydir"

Features
=======

This library is full of neat features.

Custom class initialization
---------------------------

A good common practice is to store all the options parsed
in an object. Suppose you are parsing a struct for your new
'repeatlib', which repeats a string t times. Your struct
should parse a string an integer for the number of times.

.. code-block:: cpp

   struct repeat_t {
     std::string s;
     unsigned long long t = 0;
   }

Your help string is something like that:

.. code-block:: cpp

   const char help[] = "Repeat me!\n";

Then, what your parser code would look like is this:

.. code-block:: cpp

   include "argparser.h"
   
   namespace arg = argparser;
   
   struct repeat_t {
     std::string s;
     unsigned long long t = 0;
   }
   
   int main(int argc, char** argv) {
     repeat_t repeat;
     arg::build_parser(argc, argv, repeat, help)
       .bind("string", &repeat::s,
         arg::doc("The string to be repeated"),
	     arg::def("foo"))
       .bind("times", &repeat::t,
         arg::doc("How many times your string will be repeated"),
	     arg::def(3));
       .build();
   }

That means that, by default, your program gets to repeat
"foo" 3 times. Isn't that neat?

Support for help strings in case of ill-formed arguments
--------------------------------------------------------

As seen on the previous example, we have defined a help
string for ill formed arguments. That means that if we
were to give the '--help' parameter, we should get the
help string to the screen. The program would also be
aborted immediately after.

Expandable argument deserialization
-----------------------------------

A way to customize your deserialization is by overriding
the operator>> function of your custom class. For example,
if we had a class that takes a comma separated pair of
integers, it would have its own `deserialization function
<http://www.cplusplus.com/reference/istream/istream/operator%3E%3E/>`_.
