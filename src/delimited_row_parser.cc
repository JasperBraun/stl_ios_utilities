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

#include "delimited_row_parser.h"

#include <sstream>
#include <vector>

namespace stl_ios_utilities {

namespace {

bool is_overfilled(int max_fields, int field_count) {
  return (max_fields > 0 && field_count > max_fields);
}

// if row can fit more fields, appends field to row and clears field contents
void process_field(
    std::string* field,
    std::vector<std::string>* row,
    int field_count,
    int max_fields,
    const std::unordered_map<int, std::function<void(std::string*)>>&
        field_parsers) {
  if (!is_overfilled(max_fields, field_count)) {
    if (field_parsers.count(field_count) > 0) {
      field_parsers.at(field_count)(field);
      row->push_back(std::move(*field));
    } else {
      row->push_back(std::move(*field));
    }
    (*field) = std::string{};
  }
  return;
}

} // namespace

std::istream& DelimitedRowParser::parse_row(std::istream* is,
                                            std::vector<std::string>* row) {
  std::vector<std::string> tmp_row;
  std::stringstream error_message;
  std::string field;
  int field_count{1};
  char c;

  // one by one reads letters and appends to field. Processes field and starts
  // new field when delimiter encountered, and ends processing when '\n'
  // encountered, or input stream evaluates to `false`. Checks min and max field
  // bounds where appropriate.
  while (is->get(c)) {
    if (c == '\n') {
      break;
    } else if (c == this->delimiter_) {
      process_field(&field, &tmp_row, field_count, this->max_fields_,
                    this->field_parsers_);
      field_count += 1;
      if (is_overfilled(this->max_fields_, field_count)
          && this->enforce_max_fields_) {
        error_message << "too many field(s) in input row. Expected no more than "
                      << this->max_fields_ << " fields.";
        throw UnexpectedFields(error_message.str());
      }
    } else {
      if (!is_overfilled(this->max_fields_, field_count)) {
        field.push_back(c);
      }
    }
  }
  // test min and max field bounds and process last field whose processing
  // wasn't triggered via encountering `\n`
  if (field_count < this->min_fields_ && this->enforce_min_fields_) {
    error_message << "missing field(s) in input data; detected only "
                  << field_count << " out of " << this->min_fields_
                  << " fields.";
    throw MissingFields(error_message.str());
  } else if (is_overfilled(this->max_fields_, field_count)
             && this->enforce_max_fields_) {
    error_message << "too many field(s) in input row. Expected no more than "
                  << this->max_fields_ << " fields.";
    throw UnexpectedFields(error_message.str());
  } else if ((!is_overfilled(this->max_fields_, field_count)
              || !this->ignore_overfull_row_)
             && (field_count >= this->min_fields_
                 || !this->ignore_underfull_row_)) {
    process_field(&field, &tmp_row, field_count, this->max_fields_,
                  this->field_parsers_);
    (*row) = std::move(tmp_row);
  }
  return (*is);
}

} // namespace stl_ios_utilities