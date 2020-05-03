# Design Document for `parse_delimited_row`

* [Summary](#summary)
* [Minimum required functionality](#minimum-required-functionality)
* [Design decisions](#design-decisions)
* [Options](#options)
* [Usage example](#usage-example)

Created May 01, 2020.

## Summary

The `parse_delimited_row` function reads a row of an open `std::istream` object
and parses it into a more desirable format. The different supported formats are
provided via overloads. Delimiter must be specifiable, and number of fields in
row must be enforcable.

## Minimum required functionality

* The `parse_delimited_row` function must at least work for tab-delimited files
  without header and enclosing character.
* The `parse_delimited_row` function must at least be able to read a row of data
  into a `std::vector<std::string>` object.
* The `parse_delimited_row` must be able to enforce a customizable minimum
  number of columns and a cutoff after a maximum number of columns.

## Design decisions

* Implement ability to pass custom functions to `parse_delimited_row` to be
  applied to each field for additional parsing. Should default to identity
  function.
* Minimum column number enforcement mechanism should throw exception when
  violated.
* Maximum column number enforcement mechanism should either throw exception, or
  ignore additional columns if violated, depending on user choice.
* Options are provided with a `ParseDelimitedRowOptions` object as argument
  which is implemented as `struct` with default values for each member; that
  way, more options can be added without breaking API.
  * Alternatively, the `parse_delimited_row` free function could have been
    implemented as a functor and its options as data members of the functor; I
    chose to implement it as free function with a `struct` defining the options
    passed as parameter to avoid having to construct an object before being able
    to apply the `()` operator when using default parameter values. While I have
    a feeling that this approach is the less common approach, I don't see a
    major disadvantage and will proceed with it.

## Options

Options for the `parse_delimited_row` function are passed to the function as in
the form of a `ParseDelimitedRowOptions` object. This object is implemented as a
`struct` with each option represented by one of its members.
* `delimiter`: sets the delimiting character in the input stream.
* `min_fields`: sets the minimum number of fields expected to be read.
* `max_fields`: sets the maximum number of fields expected to be read.
* `ignore_additional_fields`: determines whether additional fields are
  ignored when encountered, or whether an exception is thrown instead.
* `field_parsers`: a map from integers to function pointers indicating which
  field-parsing function should be applied to which column; column count starts
  at 0 and if no parser is assigned for a column, field is not parsed.

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