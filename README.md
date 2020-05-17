# stl_ios_utilities

v1.0.0

A C++ library containing helper functions and classes to interact with the
standard library input and output streams.

[GitHub repository.](https://github.com/JasperBraun/stl_ios_utilities.git)

[GitHub Pages.](https://jasperbraun.github.io/stl_ios_utilities/)

* [Dependencies](#dependencies)
* [Installation instructions](#installation-instructions)
* [CMake include instructions](#cmake-include-instructions)
* [Unit tests](#unit-tests)
* [API](#api)

## Dependencies

This library is written for C++ 11 and uses:
* [CMake 3.15](https://cmake.org/) or later (**optional**). This is
  optional, but this README only describes how to [comile and link the library
  using CMake and Make](#installation-instructions), and how to [add the library
  to an existing CMake project](#cmake-include-instructions).
* [GNU Make 3.81](https://www.gnu.org/software/make/) or later (**optional**).
  This is optional, but this README only describes how to [comile and link the
  library using CMake and Make](#installation-instructions).

## Installation instructions

These instructions are written for systems that support CMake and Make. Adjust
accordingly.

Clone or download and unzip this repository. Change to the cloned or
downloaded-an-unzipped `stl_ios_utilities` directory and execute these commands:

```sh
mkdir build
cd build
cmake ..
make
```

The `stl_ios_utilities` directory now contains a subdirectory called `bin` which
contains the binary for the library.

## CMake include instructions

The library target is `stl_ios_utilities` and the relevant header
`stl_ios_utilities.h` can be included from the `stl_ios_utilities/include`
directory. Depending to your project's CMake setup, the library target needs to
be linked and the `stl_ios_utilities/include` directory added to the include
directories of any target using the library.

Below is an example of how this library can be added to a target `my_target` in
a CMake project in the relevant CMakelist.txt file. Here, `<PATH_TO_LIBRARY>` is
the path to the `stl_ios_utilities` directory.

```
target_include_directories(my_target PUBLIC
        "<PATH_TO_LIBRARY>/stl_ios_utilities/include")
target_link_libraries(my_target PRIVATE stl_ios_utilities)
```

## Unit tests

To run unit tests, navigate to the `stl_ios_utilities/test` directory on the
command line and execute the following commands.

```sh
mkdir build
cd build
cmake ..
make
ctest
```

## API

Including the `stl_ios_utilities.h` header file from this library's `include`
directory will include all components of the library. Components may be included
individually, as well. All components are defined within the `stl_ios_utilities`
namespace.

The individual components are listed below with links to respective
documentations. [Detailed technical documentation for the library API can be
accessed here](https://jasperbraun.github.io/stl_ios_utilities/docs/doc/doxygen/html/index.html).

* [**`DelimitedRowParser`**](docs/delimited_row_parser.md): A Parser for reading
  from an *std::istream* object which contains rows of delimited data.
* [**`FieldParser`**](docs/field_parser.md): A parser for requesting to read any
  number of fields from an *std::istream* object which contains delimited data.