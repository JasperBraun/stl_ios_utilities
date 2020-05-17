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

#include "field_parser_test.h"

#include "field_parser.h"

#include <sstream>

namespace stl_ios_utilities {

namespace test {

namespace {

class FieldParserParserFieldsTest
    : public ::testing::TestWithParam<TestValues> {
 protected:
  std::istringstream iss;
  std::vector<std::vector<std::string>> parsed_fields;
  stl_ios_utilities::FieldParser parser;

  void SetUp() override {
    iss.str(GetParam().input_string);
    return;
  }

  void parse_fields(int field_num) {
    std::vector<std::string> fields;
    while (parser.parse_fields(&iss, &fields, field_num)) {
      if (fields.size() > 0) {
        parsed_fields.push_back(fields);
      }
      fields.clear();
    }
    if (fields.size() > 0) {
      parsed_fields.push_back(fields);
    }
  }

  void execute_with_expectations(TestPattern&& pattern_identifier,
                                 int field_num = 1) {
    if (GetParam().expectations.at(static_cast<int>(pattern_identifier))
        .throws) {
      BaseException exception
          = GetParam().expectations.at(static_cast<int>(pattern_identifier))
            .exception;
      EXPECT_THROW(try{
                     parse_fields(field_num);
                   } catch (const decltype(exception)& e) {
                     EXPECT_STREQ(e.what(), exception.what());
                     throw;
                   }, decltype(exception));
    } else {
      EXPECT_NO_THROW(parse_fields(field_num));
    }
    return;
  }

  // void TearDown() override {}
};

TEST_P(FieldParserParserFieldsTest, DefaultTest) {
  execute_with_expectations(TestPattern::kDefault);
  EXPECT_EQ(parsed_fields, GetParam().expectations.at(
      static_cast<int>(TestPattern::kDefault)).fields);
}

TEST_P(FieldParserParserFieldsTest, DelimitersTest) {
  parser.delimiters(std::unordered_set<char>{'\t', '_'});

  execute_with_expectations(TestPattern::kDelimiters, 2);
  EXPECT_EQ(parsed_fields, GetParam().expectations.at(
      static_cast<int>(TestPattern::kDelimiters)).fields);
}

TEST_P(FieldParserParserFieldsTest, TerminatorsTest) {
  parser.terminators(std::unordered_set<char>{'\n', '#'});

  execute_with_expectations(TestPattern::kTerminators, 2);
  EXPECT_EQ(parsed_fields, GetParam().expectations.at(
      static_cast<int>(TestPattern::kTerminators)).fields);
}

TEST_P(FieldParserParserFieldsTest, MaskedTest) {
  parser.masked(std::unordered_set<char>{'#'});

  execute_with_expectations(TestPattern::kMasked, 2);
  EXPECT_EQ(parsed_fields, GetParam().expectations.at(
      static_cast<int>(TestPattern::kMasked)).fields);
}

TEST_P(FieldParserParserFieldsTest, EnforceFieldNumberThrow) {
  execute_with_expectations(TestPattern::kEnforceFieldNumberThrow, 2);
  EXPECT_EQ(parsed_fields, GetParam().expectations.at(
      static_cast<int>(TestPattern::kEnforceFieldNumberThrow)).fields);
}

TEST_P(FieldParserParserFieldsTest, EnforceFieldNumberNoThrow) {
  parser.enforce_field_number(false);

  execute_with_expectations(TestPattern::kEnforceFieldNumberNoThrow, 2);
  EXPECT_EQ(parsed_fields, GetParam().expectations.at(
      static_cast<int>(TestPattern::kEnforceFieldNumberNoThrow)).fields);
}

TEST_P(FieldParserParserFieldsTest, IgnoreUnderfullData) {
  parser.enforce_field_number(false);
  parser.ignore_underfull_data(false);

  execute_with_expectations(TestPattern::kIgnoreUnderfullData, 2);
  EXPECT_EQ(parsed_fields, GetParam().expectations.at(
      static_cast<int>(TestPattern::kIgnoreUnderfullData)).fields);
}

TEST_P(FieldParserParserFieldsTest, FieldParsers) {
  std::unordered_map<int, std::function<void(std::string*)>> field_parsers{
    {1, [](std::string* s){s->append("-TEST");}},
  };
  parser.field_parsers(field_parsers);

  execute_with_expectations(TestPattern::kFieldParsers, 2);
  EXPECT_EQ(parsed_fields, GetParam().expectations.at(
      static_cast<int>(TestPattern::kFieldParsers)).fields);
}

INSTANTIATE_TEST_SUITE_P(Expected,
                         FieldParserParserFieldsTest,
                         testing::ValuesIn(kTestCases));

} // namespace

} // namespace test

} // namespace stl_ios_utilities