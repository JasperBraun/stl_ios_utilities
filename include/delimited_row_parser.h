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

/**
 * @brief A Parser for reading from an *std::istream* which contains rows of
 *  delimited data.
 *  
 * @details The `DelimitedRowParser` class reads rows of an open *std::istream*
 *  object and parses fields separated by a delimiter into a more desirable
 *  format.
 *  
 *  A row ends when a newline character is read, or the stream evaluates
 *  to `false` after a call of *std::istream::get*. The field delimiter is
 *  specifiable
 *  
 *  The number of fields that are parsed can be enforced using the
 *  `min_fields`, `enforce_min_fields`, and `ignore_underfull_row`, as well as
 *  the `max_fields`, `enforce_max_fields`, and `ignore_max_fields` methods.
 *  
 *  The `field_parsers` and `set_parsers` methods allow specification of custom
 *  string parsers to be applied to each individual column.
 *  
 *  `DelimitedRowParser` is copyable and movable.
 */
class DelimitedRowParser {
public:
  /**
   * @brief Exception thrown by Stream operations of the `DelimitedRowParser`
   *  class
   * 
   * @details Derived from *std::logic_error*.
   */
  struct MissingFields : std::logic_error {
    using std::logic_error::logic_error;
  };
  /**
   * @brief Exception thrown by Stream operations of the `DelimitedRowParser`
   *  class
   * 
   * @details Derived from *std::logic_error*.
   */
  struct UnexpectedFields : std::logic_error {
    using std::logic_error::logic_error;
  };

  /**
   * @name Parser option accessors
   */
  ///@{

  /**
   * @brief Returns the delimiter used by the `DelimitedRowParser`.
   */
  inline char delimiter() const {return delimiter_;}

  /**
   * @brief Returns the minimum number of fields expected in a data row by the
   *  `DelimitedRowParser`.
   * 
   * @see `min_fields(int min)` and `parse_row`
   */
  inline int min_fields() const {return min_fields_;}

  /**
   * @brief Indicates whether reading a data row with fewer fields than expected
   *  causes an exception of member type `MissingFields` to be thrown.
   *  
   * @see `enforce_min_fields(bool enforce)` and `parse_row`
   */
  inline bool enforce_min_fields() const {return enforce_min_fields_;}

  /**
   * @brief Indicates whether a data row with fewer fields than expected is
   *  ignored by `DelimitedRowParser`.
   *  
   * @see `ignore_undefull_row(bool ignore)` and `parse_row`
   */
  inline bool ignore_underfull_row() const {return ignore_underfull_row_;}

  /**
   * @brief Returns the maximum number of fields expected in a data row by the
   *  `DelimitedRowParser`.
   * 
   * @see `max_fields(int max)` and `parse_row`
   */
  inline int max_fields() const {return max_fields_;}

  /**
   * @brief Indicates whether reading a data row with more fields than expected
   *  causes an exception of member type `UnexpectedFields` to be thrown.
   * 
   * @see `enforce_max_fields(bool enforce)` and `parse_row`
   */
  inline bool enforce_max_fields() const {return enforce_max_fields_;}

  /**
   * @brief Indicates whether a data row with more fields than expected is
   *  ignored by `DelimitedRowParser`.
   * 
   * @see `ignore_overfull_row(bool enforce)` and `parse_row`
   */
  inline bool ignore_overfull_row() const {return ignore_overfull_row_;}
  ///@}

  /**
   * @name Parser option modifiers
   */
  ///@{
  /**
   * @brief Sets the delimiter used by the `DelimitedRowParser`.
   * 
   * @param delim The character separating field values in a data row read by
   *  `DelimitedRowParser`.
   */
  inline void delimiter(char delim) {delimiter_ = delim;}

  /**
   * @brief Sets the minimum number of fields expected in a data row by the
   *  `DelimitedRowParser`.
   *  
   * @details If a data row containing fewer fields than `min_fields_` is read,
   *  an exception may be thrown if `enforce_min_fields_` is set to `true`, or
   *  the data row may be ignored if `ignore_underfull_row_` is set to `true`.
   *  Setting `min_fields_` to `0` effectively prevents any enforcement of a
   *  minimum number of rows. Default value of `min_fields_` is `0`.
   * 
   * @param min The minimum number of fields the `DelimitedRowParser` should
   *  expect to read in one data row.
   * 
   * @see `parse_row`
   */
  inline void min_fields(int min) {min_fields_ = min;}

  /**
   * @brief Sets whether reading a data row with fewer fields than expected
   *  causes an exception of member type `MissingFields` to be thrown.
   *  
   * @details The minimum number of fields expected can be accessed/modified
   *  using `min_fields(int min)`. Default value of `enforce_min_fields_` is
   *  `true`.
   * 
   * @param enforce If set to `true` an exception of member type `MissingFields`
   *  is thrown when `DelimitedRowParser` reads a data row with fewer fields
   *  than expected.
   * 
   * @see `parse_row`
   */
  inline void enforce_min_fields(bool enforce) {enforce_min_fields_ = enforce;}

  /**
   * @brief Sets whether a data row with fewer fields than expected is ignored
   *  by `DelimitedRowParser`.
   * 
   * @details The minimum number of fields expected can be accessed/modified
   *  using `min_fields(int min)`. Default value of `ignore_underfull_row` is
   *  `true`.
   *  
   * @param ignore If set to `true` a row with fewer fields than expected is
   *  skipped by `DelimitedRowParser`.
   * 
   * @see `parse_row`
   */
  inline void ignore_underfull_row(bool ignore) {
    ignore_underfull_row_ = ignore;
  }

  /**
   * @brief Sets the maximum number of fields expected in a data row by the
   *  `DelimitedRowParser`.
   *  
   * @details If a data row containing more fields than `max_fields_` is read,
   *  an exception may be thrown if `enforce_max_fields_` is set to `true`, or
   *  the data row may be ignored if `ignore_overfull_row_` is set to `true`.
   *  Setting `max_fields_` to `0` effectively prevents any enforcement of a
   *  maximum number of rows. Default value of `max_fields_` is `0`.
   * 
   * @param max The maximum number of fields the `DelimitedRowParser` should
   *  expect to read in one data row.
   * 
   * @see `parse_row`
   */
  inline void max_fields(int max) {max_fields_ = max;}

  /**
   * @brief Sets whether reading a data row with more fields than expected
   *  causes an exception of member type `UnexpectedFields` to be thrown.
   *  
   * @details The maximum number of fields expected can be accessed/modified
   *  using `max_fields(int max)`. Default value of `enforce_max_fields_` is
   *  `true`.
   * 
   * @param enforce If set to `true` an exception of member type
   *  `UnexpectedFields` is thrown when `DelimitedRowParser` reads a data row
   *  with more fields than expected.
   * 
   * @see `parse_row`
   */
  inline void enforce_max_fields(bool enforce) {enforce_max_fields_ = enforce;}

  /**
   * @brief Sets whether a data row with more fields than expected is ignored
   *  by `DelimitedRowParser`.
   * 
   * @details The maximum number of fields expected can be accessed/modified
   *  using `max_fields(int max)`. Default value of `ignore_overfull_row` is
   *  `true`.
   *  
   * @param ignore If set to `true` a row with more fields than expected is
   *  skipped by `DelimitedRowParser`.
   * 
   * @see `parse_row`
   */
  inline void ignore_overfull_row(bool ignore) {ignore_overfull_row_ = ignore;}
  ///@}

  /**
   * @name Field parser accessors and modifiers
   */
  ///@{
  /**
   * Returns a constant reference to the *std::unordered_map* data member of
   *  `DelimitedRowParser`, which maps column numbers (starting at 1) to
   *  functions which are applied to fields of the respective columns when read
   *  by `DelimitedRowParser`. When a column is not assigned a parser, it is not
   *  parsed any further and read by `DelimitedRowParser` as is.
   * 
   * @see `parse_row`
   */
  inline const std::unordered_map<int, std::function<void(std::string*)>>&
      field_parsers() const {
    return field_parsers_;
  }

  /**
   * Returns a constant reference to the function that parses the fields in the
   *  column specified by `column` when data rows are read by
   *  `DelimitedRowParser`. Throws an exception of type *std::out_of_range* when
   *  no function is set to parse specified column.
   * 
   * @see `parse_row`
   */
  inline const std::function<void(std::string*)>& get_parser(int column) const {
    return field_parsers_.at(column);
  }

  /**
   * @brief Sets the field parsers for each column.
   * 
   * @details `DelimitedRowParser` applies the functions mapped to by the column
   *  number in the `field_parsers_`, if any, to the respective fields when
   *  reading a data row.
   * 
   * @param parsers The map with column numbers as keys (starting at 1) and
   *  functions that `DelimitedRowParser` must apply to fields from the
   *  respective columns when reading a data row. Passed as constant reference.
   */
  inline void field_parsers(
      const std::unordered_map<int, std::function<void(std::string*)>>&
          parsers) {
    field_parsers_ = parsers;
    return;
  }

  /**
   * @brief Sets the field parsers for each column.
   * 
   * @details `DelimitedRowParser` applies the functions mapped to by the column
   *  number in the `field_parsers_`, if any, to the respective fields when
   *  reading a data row.
   * 
   * @param parsers The map with column numbers as keys (starting at 1) and
   *  functions that `DelimitedRowParser` must apply to fields from the
   *  respective columns when reading a data row. Passed as rvalue reference.
   */
  inline void field_parsers(
      std::unordered_map<int, std::function<void(std::string*)>>&& parsers) {
    field_parsers_ = parsers;
    return;
  }

  /**
   * @brief Sets the function parsing specified column's field values.
   * 
   * @details `DelimitedRowParser` applies `parser` to fields in the column
   *  number `column` (starting at 1).
   * 
   * @param column Number of the column whose fields `parser` must be applied
   *  to (starting at 1).
   * 
   * @param parser The function parsing fields from the column indicated by the
   *  parameter `column`.
   */
  inline void set_parser(int column,
                         const std::function<void(std::string*)>& parser) {
    field_parsers_[column] = parser;
    return;
  }
  ///@}

  /**
   * @name Stream operations
   */
  ///@{
  /**
   * @brief Reads a data row splitting delimited columns into fields, applying
   *  parsers and stores them in provided *std::vector<std::string>* object.
   * 
   * @details Reads characters from `is` until either the `\n` character is
   *  read, or the stream evaluates to `false`. Whenever a field delimiter is
   *  read the accumulated *std::string* object is parsed using the function, if
   *  any, mapped to by the respective column number in `field_parsers_`. Fields
   *  replace the contents of `row`.
   *  
   *  Enforces minimum number of fields by throwing an exception of member type
   *  `MissingFields` if fewer fields than `min_fields_` are read, and
   *  `enforce_min_fields_` is set to `true` (default). In this case, `row`
   *  remains unchanged and `is` remains is in a valid state. If too few columns
   *  are read and `enforce_min_fields_` is set to `false`, the fields will be
   *  stored in `row` unless `ignore_underfull_row_` is set to `true` (default).
   *  In the latter case, `row` is not modified.
   *  
   *  Enforces maximum number of fields by throwing an exception of member type
   *  `UnexpectedFields` if more fields than `max_fields_` are read, and
   *  `enforce_max_fields_` is set to `true` (default). In this case, `row`
   *  remains unchanged and `is` remains is in a valid state. If too many
   *  columns are read and `enforce_min_fields_` is set to `false`, only the
   *  first `max_fields_` fields are stored in `row` unless
   *  `ignore_overfull_row_` is set to `true` (default). In the latter case,
   *  `row` is not modified.
   *  
   *  Field parsers mapped to by column numbers in `field_parsers_` may throw
   *  exceptions. Internally, this method repeatedly uses *std::istream::get* to
   *  read the row character-by-character, which may throw exceptions (see STL
   *  docs).
   *  
   *  Concurrent access to `is` may cause data races as documented in STL docs
   *  for *std::istream::get*.
   * 
   * @param is Pointer to the input stream containing delimited data.
   * @param row *std::vector<std::string>* object in which the fields are
   *  stored, if any. 
   * 
   * @return Returns a reference to the input stream `is` after extraction of
   *  the delimited row.
   */
  std::istream& parse_row(std::istream* is, std::vector<std::string>* row);
  ///@}

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