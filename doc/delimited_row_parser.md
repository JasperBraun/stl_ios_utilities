# Documentation for `DelimitedRowParser`

## Summary

The `DelimitedRowParser::parse_row` method reads a data row of an open
*std::istream* object which contains rows of delimited data field (such as in
.tsv or .csv files). Fields are stored in a *std::vector<std::string>* object.
Several options control the
[minimum and maximum numbers of fields](#control-over-field-numbers) and the
method's behavior if that expectation is violated.
Additional functions [can be specified to parse individual fields](#field-parsers).

## Basic usage

The `DelimitedRowParser::parse_row` method takes as argument a pointer to a
*std::istream* object and a pointer to a *std::vector<std::string>* object and
stores the field values of the next row of data of the stream object in the
vector.

The delimiter that separates field values is by default `\t`, and can be set
using the `delimiter(char delim)` method. It can also be accessed using the
method `delimiter()` (without arguments).

Here is an example of how the `DelimitedRowParser::parse_row` method can be used
to read and print the contents of a data file `data.csv`, line-by-line.

`data.csv`:
```
foo,bar,baz
one,two,three
```

Example 1:
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <string>
#include <vector>

int main() {
  stl_ios_utilities::DelimitedRowParser parser{};
  std::ifstream ifs{"data.csv"};
  std::vector<std::string> row;
  parser.delimiter(',');
  if (ifs.is_open()) {
    while (parser.parse_row(&ifs, &row)) {
      for (std::string field : row) {
        std::cout << ' ' << field; // process contents of row here
      }
      std::cout << std::endl;
    }
    ifs.close();
  }
  return 0;
}
```

Output:
```
 foo bar baz
 one two three

```

Using default settings, `DelimitedRowParser::parse_row` does not throw any
exceptions unless the underlying stream does when a character is read (see STL
documentation for *std::istream::get*). Specifying a different delimiter does
not change this behavior.

## Control over field numbers

Minimum and maximum expected field numbers per row can be enforced by setting
the respective expected numbers using the `min_fields(int min)` and
`max_fields(int max)` methods.

The currently set expected minimum and maximum numbers of fields can be obtained
with the methods `min_fields()` and `max_fields()` (without arguments) and is
set by default to `0`. In the case of maximum number of fields, a value of `0`
means that no maximum number of fields is enforced.

By default, a violation of minimum or maximum number of fields would result in
exceptions of member types `MissingFields` or `UnexpectedFields`, respectively,
to be thrown by the `DelimitedRowParser::parse_row` method.

For example, attempting to execute the following code to an input stream `is`
to store the next row of data in the stream in a vector of strings `row`,
results in a `MissingFields` exception to be thrown if the next row of data in
the input stream contains fewer than `4` fields.

```C++
DelimitedRowParser parser{};
parser.min_fields(4);
parser.parse_row(&is, &row);
```

If a `MissingField` or `UnexpectedFields` exception is thrown, the input vector
of strings in which the data is supposed to be stored is not modified. To avoid
throwing an exception and instead simply ignore the row of data and not
modifying the input vector of strings, one can execute the following two
methods.

```C++
parser.enforce_min_fields(false);
parser.enforce_max_fields(false);
```

If the row should not be ignored because too few fields are read, one can
execute the method `ignore_underful_row(false)` so that as many fields as
possible are read.

If the row should not be ignored because more fields than expected are read, one
can execute the method `ignore_overfull_row(false)` so that fields are read and
stored in the input vector of strings until the expected maximum number of
fields was reached.

To illustrate these methods, two examples and their outputs are shown below.
Both work off the file `irregular_data.csv`.

`irregular_data.csv`:
```
foo,bar,baz
one,two,three,four,five
too,few
```

Example 2:
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <string>
#include <vector>

int main() {
  stl_ios_utilities::DelimitedRowParser parser{};
  std::ifstream ifs{"irregular_data.csv"};
  std::vector<std::string> row;
  parser.delimiter(',');
  parser.min_fields(3);
  parser.max_fields(3);
  parser.enforce_max_fields(false);
  parser.ignore_overfull_fields(false);
  if (ifs.is_open()) {
    while (parser.parse_row(&ifs, &row)) {
      for (std::string field : row) {
        std::cout << ' ' << field; // process contents of row here
      }
      std::cout << std::endl;
    }
    ifs.close();
  }
  return 0;
}
```

Output:
```
 foo bar baz
 one two three
 // exception of type DelimitedRowParser::MissingFields
```

When the exception is thrown in the above example, `row` was not modified and
still holds the fields from the previous loop iteration (`{"one", "two",
"three"}`).

Example 3:
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <string>
#include <vector>

int main() {
  stl_ios_utilities::DelimitedRowParser parser{};
  std::ifstream ifs{"irregular_data.csv"};
  std::vector<std::string> row;
  parser.delimiter(',');
  parser.min_fields(3);
  parser.enforce_min_fields(false);
  parser.ignore_underfull_row(false);
  parser.max_fields(3);
  parser.enforce_max_fields(false);
  parser.ignore_overfull_fields(true); // redundant; true is default value
  if (ifs.is_open()) {
    while (parser.parse_row(&ifs, &row)) {
      for (std::string field : row) {
        std::cout << ' ' << field; // process contents of row here
      }
      std::cout << std::endl;
    }
    ifs.close();
  }
  return 0;
}
```

Output:
```
 foo bar baz
 foo bar baz
 too few
```

Observe that the second row in the file `irregular_data.csv` was ignored by the
`DelimitedRowParser::parse_row` method, leaving its argument `row` unchanged, so
that ` foo bar baz` was printed out a second time. Clearing fields before
processing can prevent processing the same row twice.

## Field parsers

The `DelimitedRowParser::parser_row` method can be told to apply a custom *field
parser* function to individual fields before storing the field in its
*std::vector<std::string>* argument.

A field parser is a function with signature `void foo(std::string* arg)`. A
`DelimitedRowParser` object has a data member which maps integers corresponding
to column numbers to field parsers. When a field from a column is read by
`DelimitedRowParser::parser_row` and the column number (starting at 1) maps to
one of the field parsers, the parser is applied to the field before storing it.

Below is a list of methods which access and modify the field parsers used by
a `DelimitedRowParser` object.
* `field_parsers()`: returns a map of integers (column numbers) to function
  pointers (field parsers) which indicates the parsers used by the object.
  Return type: `std::unordered_map<int, std::function<void(std::string*)>>`.
* `get_parser(int column)`: returns a function pointer to the field parser that
  the object applies to the fields in the column indicated by the specified
  column number `column`. (This method throws an *std::out_of_range* exception
  if no column parser was set for the specified column)
* `field_parsers(const std::unordered_map<int, std::function<void(std::string*)>>& parsers)`:
  sets the internal map of column numbers to field parsers to the specified
  value.
* `field_parsers(std::unordered_map<int, std::function<void(std::string*)>>&& parsers)`:
  same as the previous, except that the argument is accepted as rvalue
  reference.
* `set_parser(int column, const std::function<void(std::string*)>& parser)`:
  sets the specified field parser for the specified column.

The following example will illustrate how these field parsers work. If
unfamiliar with the lambda function syntax, for the purpose of this example,
`[](std::string* s){s->append("_a");}` is a function that appends `_a` to the
string pointed to by its argument.

`data.csv`:
```
foo,bar,baz
one,two,three
```

Example 4:
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <functional>
#include <string>
#include <vector>

int main() {
  stl_ios_utilities::DelimitedRowParser parser{};
  std::ifstream ifs{"irregular_data.csv"};
  std::vector<std::string> row;
  std::function<void(std::string*)> append_a = [](std::string* s){s->append("_a");}
  std::set_parser(2, append_a);

  std::string s{"test"};
  std::cout << parser.get_parser(2)(&s) << std::endl;

  if (ifs.is_open()) {
    while (parser.parse_row(&ifs, &row)) {
      for (std::string field : row) {
        std::cout << ' ' << field; // process contents of row here
      }
      std::cout << std::endl;
    }
    ifs.close();
  }
  return 0;
}
```

Output:
```
test_a
 foo bar_a baz
 one two_a three
```