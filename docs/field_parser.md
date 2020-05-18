# Documentation for `FieldParser`

* [Summary](#summary)
* [Basic usage](#basic-usage)
* [Control over field numbers](#control-over-field-numbers)
* [Field parsers](#field-parsers)

## Summary

This document serves as a primer on how to use the `FieldParser` class. [More
detailed technical documentation for the library API can be accessed here](https://jasperbraun.github.io/stl_ios_utilities/docs/doxygen/html/index.html).

The `FieldParser` class is designed to interact with an *std::istream* object in
which data is stored which is separated into fields delimited by one or more
delimiting character(s) (ex.: *std::ifstream* of a `.tsv` or `.csv` file). Each
of the main stream operations of the `FieldParser` class is described in its own
section and a list of short descriptions of these operations as well as links to
their respective sections are listed below.
* [**FieldParser::parse_fields**](#parse-fields): requests a specific number of
  fields from an *std::istream* object, parses the fields using custom parsers
  and stores the resulting field values in a *std::vector<std::string>* object.

## parse_fields

```C++
std::istream& parse_fields(std::istream* is,
                           std::vector<std::string>* fields,
                           int field_number = 1);
```

The argument `is` points to an *std::istream* object whose contents are read.
The resulting field values are stored in the `fields` and the argument
`field_number` determines how many fields are requested to be read. This method
returns a reference to `is` mirroring STL stream operations.

#### Basic usage

Here is an example of a comma-delimited data `data.csv` file and how the
`parse_fields` method can be used to read its fields one-by-one.

`data.csv`:
```
foo,bar,baz,bip
bor,fur,tic,toc
```

Example 1:
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

int main() {
  std::ifstream ifs{"data.csv"};
  stl_ios_utilities::DelimitedRowParser parser;
  parser.delimiters({','});
  std::vector<std::string> fields;
  if (ifs.is_open()) {
    while (parser.parse_fields(&ifs, &fields)) {
      std::cout << fields[0] << std::endl;
    }
    std::cout << fields[0] << std::endl; // while loop skips last field
  } else {
    // something went wrong opening "data.csv"
  }
  return 0;
}

/* Output:
foo
bar
baz
bip
bor
fur
tic
toc
*/
```

Here, a single field is read into `fields` one at a time until the end of the
file. Note that the contents of the `fields` argument are replaced by
`parse_fields`. Also, `parse_fields` encounters the end of the file while
reading the last field causing its return value to evaluate to `false`. For that
reason, the body of the `while` loop does not execute when `fields` contains the
last field.

#### Request multiple fields

A third parameter of type `int` allows to specify the number of fields
`parse_fields` is supposed to read from the input stream. Here is an example of
how `data.csv` (see #basic-usage) can be read in batches of 1 and 3 fields.

Example 2:
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

int main() {
  std::ifstream ifs{"data.csv"};
  ifs.exceptions(std::ifstream::eofbit);
  stl_ios_utilities::DelimitedRowParser parser;
  parser.delimiters({','});
  std::vector<std::string> fields;
  while (ifs.is_open()) {
    parser.parse_fields(&ifs, &fields); // request 1 field
    std::cout << fields[0] << ": ";
    parser.parse_fields(&ifs, &fields, 3); // request 3 fields
    std::cout << fields[0] << ' '
              << fields[1] << ' '
              << fields[2] << std::endl;
    if (ifs.eof()) {
      ifs.close();
    }
  }
  return 0;
}

/* Output:
foo: bar baz bip
bor: fur tic toc
*/
```

#### Enforcing the requested number of fields

So far the provided examples heavily relied on a nice input file and knowing
exactly how many fields are in each row of the input data. Below is a list of
exceptions `parse_fields` throws and when it throws them. All of these
exepctions are defined in the `stl_ios_utilities` namespace and derived from
`stl_ios_utilities::BaseException`, which in turn is derived from
`std::logic_error`.
* **`MissingFields`**: thrown when a number of fields is requested, but not
  found in input stream before a `terminator` is encountered. Note that by
  default, the only `terminator` is the newline character `\n`.
* **`EmptyField`**: thrown when an empty field is read by `parse_fields`. An
  empty field is a field without any data in it. That could mean two consecutive
  `delimiter` characters, or a `delimiter` character immediately followed by a
  `terminator`, or a `delimiter` being the first character read by
  `parse_fields`.
* **`InvalidArgument`**: thrown when a non-positive number of arguments is
  requested.

In addition, `parse_fields` internally makes repeated calls to
`std::istream::get`, which may throw its own exceptions, depending on user
settings applied to the input stream object. See [STL documentation for
`std::istream::get`](https://en.cppreference.com/w/cpp/io/basic_istream/get)
for more details.

A `FieldParser` object can be set to avoid throwing a `MissingFields` exception,
when executing `parse_fields` using the function member `enforce_field_number`.
This function member changes the value of the associated internal data member
`enforce_field_number_`, which is `true` by default.

Here is an example of what happens when the `FieldParser` object is set to not
throw when too few fields before the next `terminator` or end of stream are
encountered.

Example 3:
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

int main() {
  std::ifstream ifs{"data.csv"};
  stl_ios_utilities::DelimitedRowParser parser;
  parser.delimiters({','});
  std::vector<std::string> fields;

  parser.enforce_field_number(false);

  while (parser.parse_fields(&ifs, &fields, 3)) {
    std::cout << fields[0] << ' '
              << fields[1] << ' '
              << fields[2] << std::cout;
  }
  return 0;
}

/* Output:
foo bar baz
foo bar baz
bor fur tic
bor fur tic
*/
```

The second time that `parse_fields` is called in this example, `fields` contains
the three field values `"foo"`, `"bar"`, `"baz"`. After `parse_fields` reads the
next field value `"bip"`, it encounters a newline character `\n` which by
default is a terminator. Since 3 fields were requested, and only one field was
read, the function ignores the data it read and does not change the contents of
`fields`.

If fewer fields than requested are encountered and it is desirable to replace
the contents of `fields` by their values, the internal variable
`ignore_underfull_data_` can be set to false, which is `true` by default. Thus,
had the command `parser.ignore_underfull_data(false)` been executed before the
while loop in Example 3, the output would have been as shown below.
```
foo bar baz
bip
bor fur tic
toc
```

#### Delimiting, terminating, and masked characters

The `FieldParser` class has data members which define the characters that are
delimiter, terminators, or which are masked. A delimiter marks an end of a data
field and the beginning of a new field, a terminator marks a position at which
the `parse_fields` method should discontinue looking for more fields and masked
characters are simply skipped by `parse_fields`.

These characters can be set using the methods `FieldParser::delimiters`,
`FieldParser::terminators` and `FieldParser::masked`, all of which take a
*std::unordered_set<char>* object as argument, which will be the new set of
characters in the respective categories. For example executing the following
lines of code will set the delimiters to be `'\t'` and `','`, the terminators to be
`'\n'` and `'|'` and the masked characters to be `'#'` `'$'`, `'"'`.

```C++
FieldParser parser;
parser.delimiters({'\t', ','});
parser.terminators(std::set<char>{'\n', '|'});
std::set<char> masked_characters{'#', '$', '"'};
parser.masked(masked_characters);
```

To see the use of setting different delimiters, terminators and masked
characters consider the data file `strange_data.csv` shown below as well as
Example 4 which reads this data.

`strange_data.csv`
```
foo_bar|baz_bip,bo$_fur|tic_toc
top_dog|hot_rod,k#d_cat|rif_raf
```

Example 4
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

int main() {
  std::ifstream ifs{"strange_data.csv"};
  stl_ios_utilities::DelimitedRowParser parser;
  std::vector<std::string> fields;

  parser.delimiters({',', '_'});
  parser.terminators({'\n', '|'});
  parser.masked({'$', '#'});

  while (parser.parse_fields(&ifs, &fields, 2)) {
    std::cout << fields[0] << ' '
              << fields[1] << std::cout;
  }
  return 0;
}

/* Output:
foo bar
baz bip
bo fur
tic toc
top dog
hot rod
kd cat
rif raf
*/
```

#### Field parsers

To parse fields using custom parsers, a parser function with function signature
`void field_parser(std::string* s)` has to be added to the data member
`field_parsers_` of `FieldParser` which is of type
*std::unordered_map<int, std::function<void(std::string*)>>*. An object of this
type can be set as the value of `field_parsers_` using the `field_parsers`
function, as seen below.
```
FieldParser parser;
parser.field_parsers({{1, [](std::string* s){/* do something with s*/}},
                      {2, [](std::string* s){/* do something with s*/}}});
```
Starting count at 1, a field parser stored at key `i` is applies to the `i`'th
field read by `parse_row`. If no such field parser is specified, the field is
read as is.

Field parsers can also be added individually using `FieldParser::add_parser`.
There are multiple ways how a function can be passed to `add_parser`, some of
which are shown in the next example.

`six_column_data.csv`
```
foo,bar,baz,bip,bor,fur
tic,toc,top,dog,hot,rod
```

Example 5:
```C++
#include "stl_ios_utilities.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

void append_1(std::string* s) {
  s->append("_1");
  return;
}

void append_2(std::string* s) {
  s->append("_2");
}

int main() {
  std::ifstream ifs{"six_column_data.csv"};
  stl_ios_utilities::DelimitedRowParser parser;
  parser.delimiters({','});
  std::vector<std::string> fields;

  std::function<void(std::string*)> f{append_2};
  parser.add_parser(1, append_1);
  parser.add_parser(2, f);
  parser.add_parser(3, [](std::string* s){s->append("3");});

  while (parser.parse_fields(&ifs, &fields, 3)) {
    std::cout << fields[0] << ' '
              << fields[1] << ' '
              << fields[2] << std::cout;
  }
  return 0;
}

/* Output:
foo_1 bar_2 baz_3
bip_1 bor_2 fur_3
tic_1 toc_2 top_3
dog_1 hot_2 rod_3
*/
```