# Design document for `FieldParser`

## Summary

The goal is to add a method `parse_fields` which can parse a specific number of
fields from and *std::istream* object based on a provided set of
field-separating characters delimiter. Applications include but are not limited
to the following examples:
* Reading data fields where fields are nested based on different delimiters.
* Reading a delimited data row in chunks.

Terminating characters can be added to signify that the fields should be read
from the input stream only until such character is encountered.

## Minimum required functionality

* Must at least work for tab-delimited data files without enclosing characters.
* Must at least be able to read the data fields into a
  *std::vector<std::string>* object.
* User should be able to silence exceptions due to violation of field number.
  One of three of the following behaviors must be specifiable:
  * Throw exception if not enough fields were read before a terminator is read.
  * Ignore all read fields.
  * Read as many fields as possible until field number is reached.
* Must be able to define custom field parsers which are applied on individual
  fields.

## Design decisions

* Consider keeping the function extensible to become new foundation of
  `DelimitedRowParser::parse_row`, which in this context simply can be viewed as
  an application of `FieldParser::parse_fields` reading a single field with
  '\n' character as terminator and desired column number specified as expected
  field number.
* Field parsing functions are required to have function signature
  *void parser(std::string* field)* modifying the field in place.

## Options

The parameter value of `field_number` determines the number of fields expected
to be read before `terminator` (**default:** `1`).

More options for the `parse_row` class method are defined via data members and
can be viewed/modified via getters/setters. The following list contains the
options that can be specified and their default values. Note that a character
cannot be a member of more than one of `delimiters`, `terminators`, and
`masked`.
* `delimiters`: the set of field-separating characters in the input stream
  (**default:** *std::unordered_set<char>{'\t'}*).
* `terminators`: the set of characters whose encounter stops extraction of
  characters from the input stream
  (**default:** *std::unordered_set<char>{'\n'}*).
* `masked`: a set of characters which are skipped over while reading a field
  value (**default:** *std::unordered_set<char>{}*).
* `enforce_field_number`: determines whether or not fewer fields than
  `field_number` before a member of `delimiters` cause an exception to be thrown
  (**default:** `true`).
* `ignore_underfull_data`: determines whether or not fewer fields than
  `field_number` before a member of `delimiters` results in no field being read
  (**default:** `true`). Only applies when `enforce_field_number` is set to
  `false`.
* `field_parsers`: a map from integers to function pointers indicating which
  field-parsing function should be applied to which field; field count starts
  at `1` and fields with no parser assigned are read as is.
  (**default:** *std::unordered_map<int, std::function<void(std::string*)>>*).

## Usage example

Here is an example of the intended usage of the `parse_fields` class method of
the `FieldParser` class.

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