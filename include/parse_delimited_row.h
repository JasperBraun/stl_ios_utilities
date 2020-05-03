// Copyright (c) 2020 Jasper Braun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef STL_IOS_UTILITIES_PARSE_DELIMTED_ROW_H_
#define STL_IOS_UTILITIES_PARSE_DELIMTED_ROW_H_

#include <functional>
#include <istream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace stl_ios_utilities {

std::istream& parse_delimited_row(
    std::istream& is,
    vector<std::string> row,
    ParseDelimitedRowOptions options = ParseDelimitedRowOptions{});

struct MissingFields : std::logic_error {
  using std::logic_error::logic_error;
};
struct UnexpectedFields : std::logic_error {
  using std::logic_error::logic_error;
};

class ParseDelimitedRowOptions {
  char delimiter{'\t'};
  int min_fields{0};
  int max_fields{0};
  bool ignore_additional_fields{true};
  std::unordered_map<int, std::function<void(std::string*)>> field_parsers;
};

} // namespace stl_ios_utilities

#endif // STL_IOS_UTILITIES_PARSE_DELIMTED_ROW_H_