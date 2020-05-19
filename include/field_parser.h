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

#ifndef STL_IOS_UTILITIES_FIELD_PARSER_H_
#define STL_IOS_UTILITIES_FIELD_PARSER_H_

#include "exceptions.h"

#include <functional>
#include <istream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace stl_ios_utilities {

/// @ingroup Parsers
/// @brief A Parser for reading from an *std::istream* which contains fields of
///  delimited data.
///  
/// @details The `FieldParser::parse_fields` function member allows for
///  requesting a number of fields from the input stream. Options are specified
///  as data members of the class.
///
/// @usage
///
/// `data.csv`
/// ```
/// foo,bar,baz,bip
/// bor,fur,tic,toc
/// ```
/// `main.cc`
/// ```
/// #include "stl_ios_utilities.h"
///
/// #include <fstream>
/// #include <iostream>
/// #include <string>
/// #include <unordered_set>
/// #include <vector>
///
/// int main() {
///   std::ifstream ifs{"data.csv"};
///   stl_ios_utilities::DelimitedRowParser parser;
///   parser.delimiters(std::unordered_set<char>{','});
///   std::vector<std::string> fields;
///   if (ifs.is_open()) {
///     while (parser.parse_fields(&ifs, &fields, 2)) { // request 2 fields
///       std::cout << fields[0] << ' ' << fields[1] << std::endl;
///     }
///     std::cout << fields[0] << ' ' << fields[1] << std::endl; // while loop skips last field
///   } else {
///     // something went wrong opening "data.csv"
///   }
/// }
/// ```
/// Output:
/// ```
/// foo bar
/// baz bip
/// bor fur
/// tic toc
/// ``` 
///
class FieldParser {
 public:
  /// @name Constructors:
  ///
  /// @{
  
  FieldParser() = default;
  
  FieldParser(const FieldParser& other) = default;
  FieldParser(FieldParser&& other) = default;
  /// @}
  
  /// @name Assignment:
  ///
  /// @{
  
  FieldParser& operator=(const FieldParser& other) = default;
  FieldParser& operator=(FieldParser&& other) = default;
  /// @}

  /// @name Stream operations:
  ///
  /// @{
  
  /// @brief Reads a characters from `is` splitting the contents into fields.
  /// 
  /// @details Reads characters from `is` until either one of the terminators is
  ///  read, or the stream evaluates to `false`. Whenever a field delimiter is
  ///  read *std::string* object consisting of accumulated characters is parsed
  ///  using the function, if any, mapped to by the respective column number in
  ///  `field_parsers_`. Fields read replace the contents of `fields`.
  ///  Characters listed in data member `masked_` are skipped while reading from
  ///  `is`.
  ///  
  ///  Enforces requested number of fields (`field_number`) by throwing an
  ///  exception of type `stl_ios_utilities::MissingFields` if fewer fields than
  ///  than requested are read when the end of file of a terminator is read, and
  ///  `enforce_field_number_` is set to `true` (default). In this case,
  ///  `fields` remains unchanged and `is` remains in a valid state. If too few
  ///  fields are read and `enforce_field_number_` is set to `false`, the fields
  ///  will be stored in `fields` unless `ignore_underfull_data_` is set to
  ///  `true` (default). In the latter case, `fields` is not modified. An
  ///  exception of type `stl_ios_utilities::InvalidArgument` is thrown if a
  ///  non-positive value for `field_number` is passed. An exception of type
  ///  `stl_ios_utitilities::EmptyField` is thrown when a delimiter is read
  ///  before any data was read into the current field.
  ///  
  ///  Field parsers mapped to by field numbers in `field_parsers_` may throw
  ///  exceptions depending on the function. Internally, this method repeatedly
  ///  uses *std::istream::get* to read the row character-by-character, which
  ///  may throw exceptions (see STL docs).
  ///  
  ///  Concurrent access to `is` may cause data races as documented in STL docs
  ///  for *std::istream::get*.
  /// 
  /// @param is Pointer to the input stream containing delimited data.
  /// 
  /// @param fields *std::vector<std::string>* object in which the fields are
  ///  stored, if any. 
  ///
  /// @param field_number The number of fields requested to be read into
  ///  `fields` (**default:** 1). Must be positive.
  /// 
  /// @return Returns a reference to the input stream `is` after extraction of
  ///  the fields. If an exception of type `stl_ios_utilities::BaseException`
  ///  occurred, or only fewer fields than requested are read, then `is` is left
  ///  at the point where `parse_fields` stopped reading characters.
  ///
  std::istream& parse_fields const (std::istream* is,
                                    std::vector<std::string>* fields,
                                    int field_number = 1);
  /// @}

  /// @name Accessors:
  ///
  /// @{
  
  /// @brief Returns a constant reference to the object's data member
  ///  `delimiters_`.
  ///
  inline const std::unordered_set<char>& delimiters() const {
    return delimiters_;
  }

  /// @brief Returns a constant reference to the object's data member
  ///  `terminators_`.
  ///
  inline const std::unordered_set<char>& terminators() const {
    return terminators_;
  }

  /// @brief Returns a constant reference to the object's data member
  ///  `masked_`.
  ///
  inline const std::unordered_set<char>& masked() const {return masked_;}

  /// @brief Returns a copy of the value of object's data member
  ///  `enforce_field_number_`.
  ///
  inline bool enforce_field_number() const {return enforce_field_number_;}

  /// @brief Returns a copy of the value of object's data member
  ///  `ignore_underfull_data_`.
  ///
  inline bool ignore_underfull_data() const {return ignore_underfull_data_;}

  /// @brief Returns a constant reference to the object's data member
  ///  `field_parsers_`.
  ///
  inline const std::unordered_map<int, std::function<void(std::string*)>>&
      field_parsers() const {return field_parsers_;}
  /// @}

  /// @name Modifiers:
  ///
  /// @{
  
  /// @brief Sets value of object's data member `delimiters_` to `value`.
  ///
  /// @param value The new value for object's data member `delimiters_`.
  ///
  inline void delimiters(const std::unordered_set<char>& value) {
    delimiters_ = value;
  }
  
  /// @brief Sets value of object's data member `delimiters_` to `value`.
  ///
  /// @param value The new value for object's data member `delimiters_`.
  ///
  inline void delimiters(std::unordered_set<char>&& value) {
    delimiters_ = value;
  }

  /// @brief Sets value of object's data member `terminators_` to `value`.
  ///
  /// @param value The new value for object's data member `terminators_`.
  ///
  inline void terminators(const std::unordered_set<char>& value) {
    terminators_ = value;
  }
  
  /// @brief Sets value of object's data member `terminators_` to `value`.
  ///
  /// @param value The new value for object's data member `terminators_`.
  ///
  inline void terminators(std::unordered_set<char>&& value) {
    terminators_ = value;
  }

  /// @brief Sets value of object's data member `masked_` to `value`.
  ///
  /// @param value The new value for object's data member `masked_`.
  ///
  inline void masked(const std::unordered_set<char>& value) {masked_ = value;}
  
  /// @brief Sets value of object's data member `masked_` to `value`.
  ///
  /// @param value The new value for object's data member `masked_`.
  ///
  inline void masked(std::unordered_set<char>&& value) {masked_ = value;}

  /// @brief Sets value of object's data member `enforce_field_number_` to
  ///  `value`.
  ///
  /// @param value The new value for object's data member
  ///  `enforce_field_number_`.
  ///
  inline void enforce_field_number(bool value) {enforce_field_number_ = value;}

  /// @brief Sets value of object's data member `ignore_underfull_data_` to
  ///  `value`.
  ///
  /// @param value The new value for object's data member
  ///  `ignore_underfull_data_`.
  ///
  inline void ignore_underfull_data(bool value) {
    ignore_underfull_data_ = value;
  }

  /// @brief Sets value of object's data member `field_parsers_` to `value`.
  ///
  /// @param value The new value for object's data member `field_parsers_`.
  ///
  inline void field_parsers(
      const std::unordered_map<int, std::function<void(std::string*)>>& value) {
    field_parsers_ = value;
  }
  
  /// @brief Sets value of object's data member `field_parsers_` to `value`.
  ///
  /// @param value The new value for object's data member `field_parsers_`.
  ///
  inline void field_parsers(
      std::unordered_map<int, std::function<void(std::string*)>>&& value) {
    field_parsers_ = value;
  }

  /// @brief Adds a field parser to object's data member `field_parsers`.
  ///
  /// @param number The number of the field to which `value` will be applied to.
  ///
  /// @param value A field parser which will be applied to the indicated field.
  ///
  inline void add_parser(int number,
                         const std::function<void(std::string*)>& value) {
    field_parsers_[number] = value;
  }
  /// @}

 private:
  /// Set of field separating characters.
  std::unordered_set<char> delimiters_{'\t'};
  /// Set of characters which interrupt the parsing of input data.
  std::unordered_set<char> terminators_{'\n'};
  /// Set of characters which are ignored when read.
  std::unordered_set<char> masked_;
  /// Indicates whether violation of requested field numbers should cause
  /// exception to be thrown.
  bool enforce_field_number_{true};
  /// Indicates whether violation of requested field numbers should result in
  /// the read fields to be ignored.
  bool ignore_underfull_data_{true};
  /// A map from field numbers (starting at 1) to field parsing functions which
  /// are applied to the corresponding fields.
  std::unordered_map<int, std::function<void(std::string*)>> field_parsers_;
};

} // namespace stl_ios_utilities

#endif // STL_IOS_UTILITIES_FIELD_PARSER_H_