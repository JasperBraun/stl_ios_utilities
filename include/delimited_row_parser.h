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

namespace stl_ios_utilities {

class DelimitedRowParser {
public:
  struct MissingFields : std::logic_error {
    using std::logic_error::logic_error;
  };
  struct UnexpectedFields : std::logic_error {
    using std::logic_error::logic_error;
  };

  inline char delimiter() const {return delimiter_;}
  inline int min_fields() const {return min_fields_;}
  inline bool enforce_min_fields() const {return enforce_min_fields_;}
  inline bool ignore_underfull_row() const {return ignore_underfull_row_;}
  inline int max_fields() const {return max_fields_;}
  inline bool enforce_max_fields() const {return enforce_max_fields_;}
  inline bool ignore_overfull_row() const {return ignore_overfull_row_;}
  inline const std::unordered_map<int, std::function<void(std::string*)>>&
      field_parsers() const {
    return field_parsers_;
  }
  inline const std::function<void(std::string*)>& get_parser(int column) const {
    return field_parsers_.at(column);
  }

  inline void delimiter(char delim) {delimiter_ = delim;}
  inline void min_fields(int min) {min_fields_ = min;}
  inline void enforce_min_fields(bool enforce) {enforce_min_fields_ = enforce;}
  inline void ignore_underfull_row(bool ignore) {
    ignore_underfull_row_ = ignore;
  }
  inline void max_fields(int max) {max_fields_ = max;}
  inline void enforce_max_fields(bool enforce) {enforce_max_fields_ = enforce;}
  inline void ignore_overfull_row(bool ignore) {ignore_overfull_row_ = ignore;}
  inline void field_parsers(
      const std::unordered_map<int, std::function<void(std::string*)>>&
          parsers) {
    field_parsers_ = parsers;
    return;
  }
  inline void field_parsers(
      std::unordered_map<int, std::function<void(std::string*)>>&& parsers) {
    field_parsers_ = parsers;
    return;
  }
  inline void set_parser(int column,
                         const std::function<void(std::string*)>& parser) {
    field_parsers_[column] = parser;
    return;
  }

  std::istream& parse_row(std::istream* is, std::vector<std::string>* row);

private:
  char delimiter_{'\t'};
  int min_fields_{0};
  bool enforce_min_fields_{true};
  bool ignore_underfull_row_{true};
  int max_fields_{0};
  bool enforce_max_fields_{true};
  bool ignore_overfull_row_{true};
  std::unordered_map<int, std::function<void(std::string*)>> field_parsers_;
};

} // namespace stl_ios_utilities

#endif // STL_IOS_UTILITIES_PARSE_DELIMTED_ROW_H_