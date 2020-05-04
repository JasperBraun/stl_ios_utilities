# Design Document for `delimited_row_parser`

* [Summary](#summary)
* [Minimum required functionality](#minimum-required-functionality)
* [Design decisions](#design-decisions)
* [Options](#options)
* [Usage example](#usage-example)

Created May 01, 2020.

## Summary

The `delimited_row_parser` reads a row of an open `std::istream` object
and parses fields separated by a delimiter into a more desirable format. A row
ends when a newline character is read, or the stream evaluates to `false` after
call of `std::istream::get`. Delimiter must be specifiable, and number of fields
per row must be enforcable.

## Minimum required functionality

* Must at least work for tab-delimited files without header and enclosing
  character.
* Must at least be able to read a row of data into a `std::vector<std::string>`
  object.
* Must be able to enforce a minimum and a maximum number of fields.
* Must be able to define custom field parsers based for individual columns.

## Design decisions

* Minimum and maximum column number enforcement mechanisms should by default
  throw exceptions when violated.
* User should be able to silence exceptions due to violation of minimum or
  maximum column number violations and specify one of two of the following
  behaviors instead:
  * ignore entire row, or
  * read as many fields as possible until maximum column number, if any, is
    reached.
* field parsing functions are required to have function signature
  `void parser(std::string* field)` modifying the field in place.

## Options

Options for the `parse_delimited_row` function are defined via data members and
can be viewed/modified via getters/setters. The following list contains the
options that can be specified and their default values:
* `delimiter`: the delimiting character in the input stream (*default:* `\t`).
* `min_fields`: sets the minimum number of fields expected to be read
  (*default:* `0`).
* `enforce_underfull_row`: determines whether or not fewer fields than
  `min_fields` in a row cause and exception to be thrown.
  (*default:* `true`).
* `ignore_underfull_row`: determines whether or not fewer fields than
  `min_fields` in a row result in no row being read (*default:* `true`). Only
  applies when `enforce_underfull_row` is set to `false`.
* `max_fields`: sets the maximum number of fields expected to be read
  (*default:* `0`). When set to `0` or less, no maximum number of fields is
  enforced.
* `enforce_overfull_fields`: determines whether or not more fields than
  `max_fields` in a row cause an exception to be thrown (*default:* `true`).
  Only applies when `max_fields` is set to a non-negative integer.
* `ignore_overfull_fields`: determines whether or not more fields than
  `max_fields` in a row result in no row being read (*default:* `true`). Only
  applies when `max_fields` is set to a non-negative integer and
  `enforce_overfull_row` is set to `false`.
* `field_parsers`: a map from integers to function pointers indicating which
  field-parsing function should be applied to which column; column count starts
  at `1` and if no parser is assigned for a column, field is not parsed and read
  as is.

## Usage example
Here is an example of the intended usage of the `parse_delimited_row` function
(neglegting exception handling for simplicity).
```C++
#include "parse_delimited_row.h"

#include <fstream>
#include <string>
#include <vector>

int main() {
  std::ifstream ifs{"filename"};
  std::vector<std::string> row;
  stl_ios_util::ParseDelimitedRowOptions options{};
  options.delimiter = ',';
  options.min_fields = 3;
  options.max_fields = 3;
  options.ignore_additional_fields = true;
  if (ifs.is_open()) {
    while (stl_ios_util::parse_delimited_row(ifs, row, options)) {
      for (auto field : row) {
        std::cout << ' ' << field;
      }
      std::cout << std::endl;
    }
  }
  return 0;
}
```

Here is an example input file for the usage above.
```
foo,bar,baz
one, two, three, four, five
too,few
```

When applying the usage example code to the example input file, the first two
lines are read properly, but the last two fields of the second line are cut off.
Also, not that the extra space before each of the field values in the second
line is read as part of the field value. When reading the third line, an
exception is thrown because the minimum field number was violated.
```
 foo bar baz
 one  two  three
/* exception occurs here */
```