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

#include "parse_delimited_row.h"

#include <sstream>
#include <vector>

namespace stl_ios_utilities {

void process_field_(
    std::string* field,
    const std::unordered_map<int, std::function<void(std::string*)>>&
        field_parsers,
    const int& field_count,
    std::vector<std::string>* row);

std::istream& parse_delimited_row(
    std::istream* is,
    std::vector<std::string>* row,
    const ParseDelimitedRowOptions& options) {
  int field_count{1};
  std::string field;
  char c;
  std::vector<std::string> tmp_row;
  std::stringstream message;

  while (is->get(c)) {
    if (c == '\n') {
      break;
    } else if (c == options.delimiter) {
      if (options.max_fields == -1 || field_count <= options.max_fields) {
        process_field_(&field, options.field_parsers, field_count, &tmp_row);
      }
      field_count += 1;
      if (field_count > options.max_fields
          && !options.ignore_additional_fields) {
        throw UnexpectedFields("additional field(s) in input data.");
      }
    } else {
      if (options.max_fields == -1 || field_count <= options.max_fields) {
        field.push_back(c);
      }
    }
  }
  if (options.max_fields == -1 || field_count <= options.max_fields)
  process_field_(&field, options.field_parsers, field_count, &tmp_row);
  if (field_count < options.min_fields) {
    message << "missing field(s) in input data; detected only "
            << field_count << " out of " << options.min_fields
            << " fields.";
    throw MissingFields(message.str());
  }

  (*row) = std::vector<std::string>{std::move(tmp_row)};
  return (*is);
}

void process_field_(
    std::string* field,
    const std::unordered_map<int, std::function<void(std::string*)>>&
        field_parsers,
    const int& field_count,
    std::vector<std::string>* row) {
  if (field_parsers.count(field_count) > 0) {
    field_parsers.at(field_count)(field);
  }
  row->push_back(std::string(std::move(*field)));
  field->clear();
  return;
}

} // namespace stl_ios_utilities