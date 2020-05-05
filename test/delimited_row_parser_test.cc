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

#include "delimited_row_parser.h"

#include <sstream>

namespace stl_ios_utilities {

namespace {

class DelimitedRowParserOptions : public ::testing::Test {
protected:
  stl_ios_utilities::DelimitedRowParser parser{};

  // void SetUp() override {}
  // void TearDown() override {}
};

class DelimitedRowParserParseRow : public ::testing::Test {
protected:
  stl_ios_utilities::DelimitedRowParser parser{};
  std::istringstream iss{};
  std::vector<std::vector<std::string>> expected{}, result{};

  void SetUp() override {}

  void SetUp(std::string&& data,
             std::initializer_list<std::initializer_list<std::string>> rows) {
    iss.str(data);
    for (const std::initializer_list<std::string>& row : rows) {
      expected.push_back(std::vector<std::string>{row});
    }
    return;
  }

  void read_data() {
    std::vector<std::string> row;
    while (parser.parse_row(&iss, &row)) {
      result.push_back(row);
    }
    result.push_back(row);
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
      while (result_row_it != (*result_it).end()) {
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

TEST_F(DelimitedRowParserOptions, Delimiter) {
  EXPECT_EQ('\t', parser.delimiter());
  parser.delimiter(' ');
  EXPECT_EQ(' ', parser.delimiter());
}

TEST_F(DelimitedRowParserOptions, MinFields) {
  EXPECT_EQ(0, parser.min_fields());
  parser.min_fields(15);
  EXPECT_EQ(15, parser.min_fields());
}

TEST_F(DelimitedRowParserOptions, EnforceMinFields) {
  EXPECT_TRUE(parser.enforce_min_fields());
  parser.enforce_min_fields(false);
  EXPECT_FALSE(parser.enforce_min_fields());
}

TEST_F(DelimitedRowParserOptions, IgnoreUnderfullRows) {
  EXPECT_TRUE(parser.ignore_underfull_row());
  parser.ignore_underfull_row(false);
  EXPECT_FALSE(parser.ignore_underfull_row());
}

TEST_F(DelimitedRowParserOptions, MaxFields) {
  EXPECT_EQ(0, parser.max_fields());
  parser.max_fields(15);
  EXPECT_EQ(15, parser.max_fields());
}

TEST_F(DelimitedRowParserOptions, EnforceMaxFields) {
  EXPECT_TRUE(parser.enforce_max_fields());
  parser.enforce_max_fields(false);
  EXPECT_FALSE(parser.enforce_max_fields());
}

TEST_F(DelimitedRowParserOptions, IgnoreOverfullRows) {
  EXPECT_TRUE(parser.ignore_overfull_row());
  parser.ignore_overfull_row(false);
  EXPECT_FALSE(parser.ignore_overfull_row());
}

TEST_F(DelimitedRowParserOptions, FieldParsers) {
  // test default
  std::unordered_map<int, std::function<void(std::string*)>> test_parsers;
  try {
    parser.get_parser(0);
  } catch (const std::out_of_range& e) {
    EXPECT_STREQ(e.what(), "unordered_map::at: key not found");
  }

  std::function<void(std::string*)> append_a
      = [](std::string* s){s->append("_a");};
  std::function<void(std::string*)> append_b
      = [](std::string* s){s->append("_b");};

  // test setting parser via set_parser method
  std::string s1{"foo"}, s2{"bar"}, s3{"baz"};
  test_parsers[0] = append_a;
  parser.set_parser(0, append_a);
  parser.get_parser(0)(&s1);
  EXPECT_EQ(s1, std::string{"foo_a"});
  parser.field_parsers().at(0)(&s1);
  EXPECT_EQ(s1, std::string{"foo_a_a"});

  // test setting parser via field_parsers method with move semantics
  test_parsers.erase(0);
  test_parsers[-5] = append_b;
  test_parsers[1] = append_a;
  test_parsers[2] = append_a;
  parser.field_parsers(std::move(test_parsers));
  test_parsers = std::unordered_map<int, std::function<void(std::string*)>>{
    {-5, append_b}, {1, append_a}, {2, append_a}
  };
  parser.field_parsers().at(-5)(&s2);
  parser.field_parsers().at(1)(&s2);
  parser.field_parsers().at(2)(&s2);
  EXPECT_EQ(s2, std::string{"bar_b_a_a"});
  try {
    parser.get_parser(0);
  } catch (const std::out_of_range& e) {
    EXPECT_STREQ(e.what(), "unordered_map::at: key not found");
  }

  // test setting parser via field_parsers method without move semantics
  test_parsers.clear();
  test_parsers[0] = append_b;
  test_parsers[3] = append_b;
  test_parsers[4] = append_a;
  parser.field_parsers(test_parsers);
  parser.field_parsers().at(0)(&s3);
  parser.field_parsers().at(3)(&s3);
  parser.field_parsers().at(4)(&s3);
  EXPECT_EQ(s3, std::string{"baz_b_b_a"});
  try {
    parser.get_parser(-5);
  } catch (const std::out_of_range& e) {
    EXPECT_STREQ(e.what(), "unordered_map::at: key not found");
  }
}

TEST_F(DelimitedRowParserParseRow, ExpectedDefault) {
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

TEST_F(DelimitedRowParserParseRow, ExpectedDelimiter) {
  SetUp("foo,bar,baz\n"
        "one, two , three\n"
        "x,y,z", {
          {"foo", "bar", "baz"},
          {"one", " two ", " three"},
          {"x", "y", "z"}
        });
  parser.delimiter(',');
  read_data();
  compare_data();
}

TEST_F(DelimitedRowParserParseRow, UnexpectedEnforceMinFields) {
  SetUp("foo\tbar\tbaz\n"
        "one\t two \t three\n"
        "x\ty", {
          {"foo", "bar", "baz"},
          {"one", " two ", " three"}
        });
  parser.min_fields(3);
  parser.enforce_min_fields(true);
  parser.ignore_underfull_row(true);
  try {
    read_data();
  } catch (const stl_ios_utilities::DelimitedRowParser::MissingFields& e) {
    EXPECT_STREQ(e.what(), "missing field(s) in input data; "
                           "detected only 2 out of 3 fields.");
  }
  compare_data();
}

TEST_F(DelimitedRowParserParseRow, UnexpectedIgnoreUnderfullRows) {
  SetUp("foo\tbar\tbaz\n"
        "one\t three\n"
        "x\ty\tz", {
          {"foo", "bar", "baz"},
          {"foo", "bar", "baz"},
          {"x", "y", "z"}
        });
  parser.min_fields(3);
  parser.enforce_min_fields(false);
  parser.ignore_underfull_row(true);
  read_data();
  compare_data();
}

TEST_F(DelimitedRowParserParseRow, UnexpectedMinFields) {
  SetUp("foo\tbar\tbaz\n"
        "one\t three\n"
        "x\ty\tz", {
          {"foo", "bar", "baz"},
          {"one", " three"},
          {"x", "y", "z"}
        });
  parser.min_fields(3);
  parser.enforce_min_fields(false);
  parser.ignore_underfull_row(false);
  read_data();
  compare_data();
}

TEST_F(DelimitedRowParserParseRow, UnexpectedEnforceMaxFields) {
  SetUp("foo\tbar\n"
        "one\t two \t three\n"
        "x\ty", {
          {"foo", "bar"}
        });
  parser.max_fields(2);
  parser.enforce_max_fields(true);
  parser.ignore_overfull_row(true);
  try {
    read_data();
  } catch (const stl_ios_utilities::DelimitedRowParser::UnexpectedFields& e) {
    EXPECT_STREQ(e.what(), "too many field(s) in input row. Expected no more "
                           "than 2 fields.");
  }
  compare_data();
}

TEST_F(DelimitedRowParserParseRow, UnexpectedIgnoreOverfullRows) {
  SetUp("foo\tbar\n"
        "one\t two \t three\n"
        "x\ty", {
          {"foo", "bar"},
          {"foo", "bar"},
          {"x", "y"}
        });
  parser.max_fields(2);
  parser.enforce_max_fields(false);
  parser.ignore_overfull_row(true);
  read_data();
  compare_data();
}

TEST_F(DelimitedRowParserParseRow, UnexpectedMaxFields) {
  SetUp("foo\tbar\tbaz\n"
        "one\t three\n"
        "x\ty\tz", {
          {"foo", "bar"},
          {"one", " three"},
          {"x", "y"}
        });
  parser.max_fields(2);
  parser.enforce_max_fields(false);
  parser.ignore_overfull_row(false);
  read_data();
  compare_data();
}

TEST_F(DelimitedRowParserParseRow, ExpectedFieldParsers) {
  SetUp("foo\tbar\tbaz\n"
        "one\t two \t three\n"
        "x\ty\tz", {
          {"foo", "bar_parsed", "baz"},
          {"one", " two _parsed", " three"},
          {"x", "y_parsed", "z"}
        });
  parser.set_parser(2, [](std::string* s){s->append("_parsed");});
  read_data();
  compare_data();
}

} // namespace

} // namespace stl_ios_utilities