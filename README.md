# stl_ios_utilities

A C++ library in which helper functions and classes to interact with the
standard library input and output streams are collected.

See more documentation on the API on the [GitHub Pages site for this project](JasperBraun.github.io/stl_ios_utilities)

* [Dependencies](#dependencies)
* [Installation instructions](#installation-instructions)
* [CMake include instructions](#cmake-include-instructions)
* [Unit tests](#unit-tests)

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

On the command line navigate to the directory, where the library should be
placed. By entering the commands below, a directory named `stl_ios_utilities`
is created. This directory contains a subdirectory called `bin` which contains
the binary for the library. Instead of the first command the repositories `.zip`
file can be downloaded and unpacked.

```sh
git clone https://github.com/JasperBraun/stl_ios_utilities.git
cd stl_ios_utilities
mkdir build
cd build
cmake ..
make
```

## CMake include instructions

The library target is `stl_ios_utilities` and the relevant header
`stl_ios_utilities.h` can be included from the `stl_ios_utilities/include`
directory. Depending to your project's CMake setup, the library needs to be
linked and its include directory added to any target using the library.
Below is an example of how this library can be added to a CMake project in the
relevant CMakelist.txt file. Here, `<TARGET_NAME>` is the name of the target that
uses `stl_ios_utilities` and `<PATH_TO_LIBRARY>` is the path to the
`stl_ios_utilities` directory.

```
target_include_directories(<TARGET_NAME> PUBLIC
        "<PATH_TO_LIBRARY>/stl_ios_utilities/include")
target_link_libraries(<TARGET_NAME> PRIVATE stl_ios_utilities)
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