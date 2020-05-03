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

/** @file parse_delimited_row.h
 * @brief [brief description]
 * 
 * Details.
 */

/** @fn parse_delimited_row(std::istream* is, std::vector<std::string>* row, const ParseDelimitedRowOptions& options = ParseDelimitedRowOptions{})
 * @brief function
 * 
 * @param is some details.
 */

#ifndef STL_IOS_UTILITIES_PARSE_DELIMTED_ROW_H_
#define STL_IOS_UTILITIES_PARSE_DELIMTED_ROW_H_

#include <functional>
#include <istream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @namespace stl_ios_utilities
 *  * `parse_delimited_row` reads a row of delimted data from a `std::istream`
 *    object and parses field values.
 */
namespace stl_ios_utilities {

/**
 * @brief Stores options for the **parse_delimited_row** function.
 * 
 * * **delimiter** defines the field-separator in the input data. *Default:*
 *      '\t'.
 * * **min_fields** defines the minimum number of fields
 *      `parse_delimited_row` will look for; if fewer fields are read
 *      before the next newline character, an exception of type
 *      `stl_ios_utilities::MissingFields` is thrown.
 * * **max_fields** defines the maximum number of fields
 *      `parse_delimited_row` will look for; if more fields are read before
 *      the next newline character, and if `ignore_additional_fields` is
 *      set to `true`, an exception of type
 *      `stl_ios_utilities::UnexpectedFields` is thrown. If set to -1 (default)
 *      no maximum field number is enforced.
 * * **ignore_additional_fields**
 */
struct ParseDelimitedRowOptions {
  char delimiter{'\t'};
  int min_fields{-1};
  int max_fields{-1};
  bool ignore_additional_fields{true};
  std::unordered_map<int, std::function<void(std::string*)>> field_parsers;
};

std::istream& parse_delimited_row(
    std::istream* is,
    std::vector<std::string>* row,
    const ParseDelimitedRowOptions& options = ParseDelimitedRowOptions{});

struct MissingFields : std::logic_error {
  using std::logic_error::logic_error;
};
struct UnexpectedFields : std::logic_error {
  using std::logic_error::logic_error;
};

} // namespace stl_ios_utilities

#endif // STL_IOS_UTILITIES_PARSE_DELIMTED_ROW_H_