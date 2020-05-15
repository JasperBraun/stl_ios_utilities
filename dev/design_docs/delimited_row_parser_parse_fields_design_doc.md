# Design document for `DelimitedRowParser::parse_fields`

## Summary

The goal is to add a method which can parse a specific number of fields based on
the provided field delimiter. Applications include but are not limited to the
following examples:
* Reading nested fields with where different levels of nesting have distinct
  delimiters.
* Reading a delimited data row in chunks.

Fields are read and parsed based on the delimiter. Encountering a newline
character before the desired number of fields is read should cause an exception
to be thrown. The parser can also be set to either throw or not throw an
exception if a newline character terminates the last desired (nonempty) field.

## Minimum required functionality

* Must at least work for tab-delimited data files without enclosing characters.
* Must at least be able to read the data fields into a
  *std::vector<std::string>* object.
* Must be able to specify whether or not exception is thrown when last field is
  not empty and terminated by newline character instead of delimiter.
* User should be able to silence exceptions due to violation of field number.
  One of two of the following behaviors must be specifiable:
  * ignore all read fields, or
  * read as many fields as possible until field number is reached.
* Must be able to define custom field parsers based on individual columns.

## Design decisions

* Consider keeping the function extensible to become new foundation of
  `parse_row`, which in this context simply can be viewed as an application of
  `parse_fields` reading a single field with newline character as delimiter.
* Field parsing functions are required to have function signature
  `void parser(std::string* field)` modifying the field in place.

## Options

Options for the `parse_row` class method are defined via data members and
can be viewed/modified via getters/setters. The following list contains the
options that can be specified and their default values:
* `delimiter`: the delimiting character in the input stream (**default:** `\t`).
* `terminator`: the character signaling end of relevant data in the input stream
  (**default:** '\n').
* `field_number`: sets the number of fields expected to be read before
  `terminator` (**default:** `1`). Can also be passed as parameter to
  `parse_row`, which does not modify this data member.
* `enforce_field_number`: determines whether or not fewer fields than
  `field_number` before `terminator` cause exception to be thrown
  (**default:** `true`).
* `ignore_field_absence`: determines whether or not fewer fields than
  `field_number` before `terminator` results in no field being read
  (**default:** `true`). Only applies when `enforce_field_number` is set to
  `false`.
* `field_parsers`: a map from integers to function pointers indicating which
  field-parsing function should be applied to which field; field count starts
  at `1` and fields with no parser assigned are read as is.
  (**default:** *std::unordered_map<int, std::function<void(std::string*)>>*).

## Usage example

Here is an example of the intended usage of the `parse_fields` class method of
the `DelimitedRowParser` class (neglegting exception handling for simplicity).

```C++
#include "delimited_row_parser.h"

#include <iostream>
#include <sstream>

int main() {
  std::istringstream iss{"foo\tbar\tbaz\tbip\n"
                         "bor\tfur\ttic\ttoc"};
  stl_ios_utilities::DelimitedRowParser parser;
  parser.add_parser(1, [](std::string* s){s->append("_1");});
  parser.add_parser(2, [](std::string* s){s->append("_2");});
  std::vector<std::string> fields;
  while (parser.parse_fields(&iss, &fields, 2)) {
    std::cout << fields[0] << ' ' << fields[1] << std::endl;
  }
}

// output:
// foo_1 bar_2
// baz_1 bip_2
// bor_1 fur_2
// tic_1 toc_2
```