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

#include "gtest/gtest.h"

#include "parse_delimited_row.h"

#include <sstream>

namespace {

/*
int main() {
  std::ifstream ifs{"filename"};
  std::vector<std::string> row;
  stl_ios_util::ParseDelimitedRowOptions options{};
  options.delimiter = ',';
  options.min_fields = 3;
  options.max_fields = 3;
  options.ignore_additional_fields = true;
  if (ifs.is_open()) {
    while (stl_ios_util::parse_delimited_row(ifs, row, options)) {
      for (auto field : row) {
        std::cout << ' ' << field;
      }
      std::cout << std::endl;
    }
  }
  return 0;
}
*/

class ParseDelimitedRowTest : public ::testing::Test {
protected:
  stl_ios_utilities::ParseDelimitedRowOptions options{};
  std::istringstream iss{};
  std::vector<std::vector<std::string>> expected{}, result{};

  void SetUp(std::string&& data,
             std::initializer_list<std::initializer_list<std::string>> data
             ) override {
    iss.str(data);
    for (std::initializer_list<std::string>& row : data) {
      expected.push_back(std::vector<std::string>{row});
    }
    return;
  }

  void read_data() {
    std::vector<std::string> row;
    while (stl_ios_utilities::parse_delimited_row(iss, row, options)) {
      result.push_back(row);
    }
    return;
  }

  void compare_data() {
    ASSERT_EQ(expected.size(), result.size());
    std::vector<std::vector<std::string>>::iterator expected_it, result_it;
    std::vector<std::string>::iterator expected_row_it, result_row_it;
    expected_it = expected.begin();
    result_it = result.begin();
    while (result_it != result.end()) {
      ASSERT_EQ((*expected_it).size(), (*result_it).size());
      expected_row_it = (*expected_it).begin();
      result_row_it = (*result_it).begin();
      while (result_row_it != (*result_it).size()) {
        EXPECT_EQ((*expected_row_it), (*result_row_it));
        expected_row_it++;
        result_row_it++;
      }
      expected_it++;
      result_it++;
    }
    return;
  }

  // void TearDown() override {}
};

TEST_F(ParseDelimitedRowTest, expected_default) {
  SetUp("foo\tbar\tbaz\n"
        "one\t two \t three\n"
        "x\ty\tz", {
          {"foo", "bar", "baz"},
          {"one", " two ", " three"},
          {"x", "y", "z"}
        });
  read_data();
  compare_data();
}

TEST_F(ParseDelimitedRowTest, expected_delimiter) {
  SetUp("foo,bar,baz\n"
        "one, two , three\n"
        "x,y,z", {
          {"foo", "bar", "baz"},
          {"one", " two ", " three"},
          {"x", "y", "z"}
        });
  options.delimiter = ',';
  read_data();
  compare_data();
}

TEST_F(ParseDelimitedRowTest, unexpected_min_fields) {
  SetUp("foo\tbar\tbaz\n"
        "one\t two \t three\n"
        "x\ty", {
          {"foo", "bar", "baz"},
          {"one", " two ", " three"}
        });
  options.min_fields = 3;
  try {
    read_data();
  } catch (const stl_ios_utilities::MissingFields& e) {
    EXPECT_EQ(e.what(), "missing column 3 in row 3 of input data.");
  }
  compare_data();
}

TEST_F(ParseDelimitedRowTest, expected_max_fields) {
  SetUp("foo\tbar\tbaz\n"
        "one\t two \t three\n"
        "x\ty\tz", {
          {"foo", "bar"},
          {"one", " two "},
          {"x", "y"}
        });
  options.max_fields = 2;
  read_data();
  compare_data();
}

TEST_F(ParseDelimitedRowTest, unexpected_ignore_additional_fields) {
  SetUp("foo\tbar\n"
        "one\t two \t three\n"
        "x\ty", {
          {"foo", "bar"}
        });
  options.max_fields = 2;
  try {
    read_data();
  } catch (const stl_ios_utilities::UnexpectedFields& e) {
    EXPECT_EQ(e.what(), "additional field(s) in row 2 of input data.");
  }
  compare_data();
}

TEST_F(ParseDelimitedRowTest, expected_field_parsers) {
  SetUp("foo\tbar\tbaz\n"
        "one\t two \t three\n"
        "x\ty\tz", {
          {"foo", "bar_parsed", "baz"},
          {"one", " two _parsed", " three"},
          {"x", "y_parsed", "z"}
        });
  options.field_parsers[1] = [](std::string* s){s.append("_parsed");};
  read_data();
  compare_data();
}

} // namespace