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

#include "field_parser.h"

#include <functional>
#include <istream>
#include <string>
#include <unordered_set>
#include <vector>

namespace stl_ios_utilities {

namespace {

// parses field, if necessary, appends to fields, and clears field contents
void process_field(
    std::string* field,
    std::vector<std::string>* fields,
    int field_count,
    const std::unordered_map<int, std::function<void(std::string*)>>&
        field_parsers) {
  if (field->length() == 0) {
    throw EmptyField("No data read, after begin of execution of"
                     " `stl_ios_utilities::parse_fields`, or after delimiter"
                     " and before next delimiter, or terminator.");
  } else if (field_parsers.count(field_count) > 0) {
    field_parsers.at(field_count)(field);
    fields->push_back(std::move(*field));
  } else {
    fields->push_back(std::move(*field));
  }
  (*field) = std::string{};
  return;
}

} // namespace

std::istream& FieldParser::parse_fields (std::istream* is,
                                         std::vector<std::string>* fields,
                                         int requested_field_number) const {
  if (requested_field_number < 1) {
    throw InvalidArgument("Must request a positive number of fields in"
                          "`stl_ios_utilities::FieldParser::parse_fields`.");
  }
  std::vector<std::string> tmp_fields;
  std::string field;
  int field_count{0};
  char c;

  // Reads letters one by one and appends to field. If delimiter encountered,
  // processes field and starts new field. If terminator encountered, or input
  // stream evaluates to `false`, stops extracting letters.
  while (is->get(c)) {
    if ((this->terminators_).count(c) == 1) {
      break;
    } else if ((this->delimiters_).count(c) == 1) {
      field_count += 1;
      process_field(&field, &tmp_fields, field_count, this->field_parsers_);
      if (field_count == requested_field_number) {
        break;
      }
    } else if ((this->masked_).count(c) == 0) {
      field.push_back(c);
    }
  }

  // Test field count and process last field whose processing wasn't triggered
  // via encountering terminator or stream evaluating to `false`.
  if (field_count < requested_field_number) {
    field_count += 1;
    process_field(&field, &tmp_fields, field_count, this->field_parsers_);
  }
  if (field_count < requested_field_number && this->enforce_field_number_) {
    throw MissingFields("Too many fields requested by"
                        " `stl_ios_utilities::FieldParser::parse_row`.");
  } else if (
      (field_count < requested_field_number && !(this->ignore_underfull_data_))
      || field_count == requested_field_number) {
    (*fields) = std::move(tmp_fields);
  }
  return (*is);
}

} // namespace stl_ios_utilities