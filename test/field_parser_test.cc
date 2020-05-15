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

struct TestPatternExpectations {
  std::vector<std::vector<std::string>> fields;
  bool throws;
  std::string message;
};

struct TestValues {
  std::string input_string;
  std::unordered_map<std::string, TestPatternExpectations> expectations;
};

TestPatternExpectations make_expectations(
    std::vector<std::vector<std::string>> expected_fields,
    bool throws_exception = false,
    std::string error_message = "") {
  TestPatternExpectations expectations;
  expectations.fields = expected_fields;
  expectations.throws = throws_exception;
  expectations.message = error_message;
  return expectations;
}

TestValues make_test_values(
    std::string&& input,
    std::unordered_map<std::string, TestPatternExpectations>&&
        pattern_expectations) {
  TestValues values;
  values.input_string = input;
  values.expectations = pattern_expectations;
  return values;
}

std::vector<TestValues> kTestCases{
    make_test_values(
        "foo_bar\tbaz_bum\tbel_bol\tr#f_h#d\n"
        "pif_paf\t#ic_fi#\ttin_tin\tti#_#do\n"
        "sin_cos\tt#c_f#p\trif_raf",
        std::unordered_map<std::string, TestPatternExpectations>{
            {"default", make_expectations(
                std::vector<std::vector<std::string>>{
                    {"foo_bar"}, {"baz_bum"}, {"bel_bol"}, {"r#f_h#d"}
                    {"pif_paf"}, {"#ic_fi#"}, {"tin_tin"}, {"ti#_#do"},
                    {"sin_cos"}, {"t#c_f#p"}, {"rif_raf"}
                }, false, "")},
            {"delimiters", make_expectations(
                std::vector<std::vector<std::string>>{
                    {"foo", "bar"}, {"baz", "bum"}, {"bel", "bol"},
                    {"r#f", "h#d"}, {"pif", "paf"}, {"#ic", "fi#"},
                    {"tin", "tin"}, {"ti#", "#do"}, {"sin", "cos"},
                    {"t#c", "f#p"}, {"rif", "raf"}
                }, false, "")},
            {"terminators", make_expectations(
                std::vector<std::vector<std::string>>{
                    {"foo_bar", "baz_bum"}
                }, true, "Unexpected number of fields before terminating"
                         " character was read.")},
            {"masked", make_expectations(
                std::vector<std::vector<std::string>>{
                    {"foo_bar", "baz_bum"}, {"bel_bol", "rf_hd"}
                    {"pif_paf", "ic_fi"}, {"tin_tin", "ti_do"},
                    {"sin_cos", "tc_fp"}
                }, true, "Unexpected number of fields before terminating"
                         " character was read.")},
            {"enforce_field_number_throw", make_expectations(
                std::vector<std::vector<std::string>>{
                    {"foo_bar", "baz_bum"}, {"bel_bol", "r#f_h#d"}
                    {"pif_paf", "#ic_fi#"}, {"tin_tin", "ti#_#do"},
                    {"sin_cos", "t#c_f#p"}
                }, true, "Unexpected number of fields before terminating"
                         " character was read.")},
            {"enforce_field_number_no_throw", make_expectations(
                std::vector<std::vector<std::string>>{
                    {"foo_bar", "baz_bum"}, {"bel_bol", "r#f_h#d"}
                    {"pif_paf", "#ic_fi#"}, {"tin_tin", "ti#_#do"},
                    {"sin_cos", "t#c_f#p"}
                }, false, "")},
            {"ignore_underfull_data", make_expectations(
                std::vector<std::vector<std::string>>{
                    {"foo_bar", "baz_bum"}, {"bel_bol", "r#f_h#d"}
                    {"pif_paf", "#ic_fi#"}, {"tin_tin", "ti#_#do"},
                    {"sin_cos", "t#c_f#p"}, {"rif_raf"}
                }, true, "Unexpected number of fields before terminating"
                         " character was read.")},
            {"field_parsers", make_expectations(
                std::vector<std::vector<std::string>>{
                    {"foo_bar-TEST", "baz_bum"}, {"bel_bol-TEST", "r#f_h#d"}
                    {"pif_paf-TEST", "#ic_fi#"}, {"tin_tin-TEST", "ti#_#do"},
                    {"sin_cos-TEST", "t#c_f#p"}
                }, true, "Unexpected number of fields before terminating"
                         " character was read.")}
        }
    )
}

class FieldParserParserFieldsTest
    : public ::testing::TestWithParam<TestValues> {
 protected:
  std::istringstream iss;
  std::vector<std::vector<std::string>> parsed_fields;
  stl_ios_utilities::DelimitedRowParser parser;

  void SetUp() override {
    iss.str = GetParam().input_string;
    return;
  }

  void parse_fields(int field_num = 2) {
    std::vector<std::string> fields;
    while (parser.parse_fields(&iss, &fields, int field_number = field_num)) {
      if (fields.size() > 0) {
        parsed_fields.push_back(fields);
      }
      fields.clear();
    }
    return;
  }

  // void TearDown() override {}
};

TEST_P(FieldParserParserFieldsTest, DefaultTest) {
  try{
    parse_fields(1);
  } catch (const stl_ios_utilities::BaseException& e) {
    if (GetParam().expected.throws) {
      EXPECT_EQ(std::string(e.what()),
          GetParam().expectations["default"].message);
    } else {
      throw;
    }
  }
  EXPECT_EQ(parsed_fields, GetParam().expectations["default"].fields);
}

TEST_P(FieldParserParserFieldsTest, DelimitersTest) {
  parser.delimiters(std::unordered_set<char>{'\t', '_'});
  try{
    parse_fields();
  } catch (const stl_ios_utilities::BaseException& e) {
    if (GetParam().expectations["delimiters"].throws) {
      EXPECT_EQ(std::string(e.what()),
          GetParam().expectations["delimiters"].message);
    } else {
      throw;
    }
  }
  EXPECT_EQ(parsed_fields, GetParam().expectations["delimiters"].fields);
}

TEST_P(FieldParserParserFieldsTest, TerminatorsTest) {
  parser.delimiters(std::unordered_set<char>{'\n', '#'});
  try{
    parse_fields();
  } catch (const stl_ios_utilities::BaseException& e) {
    if (GetParam().expectations["terminators"].throws) {
      EXPECT_EQ(std::string(e.what()),
          GetParam().expectations["terminators"].message);
    } else {
      throw;
    }
  }
  EXPECT_EQ(parsed_fields, GetParam().expectations["terminators"].fields);
}

TEST_P(FieldParserParserFieldsTest, MaskedTest) {
  parser.masked(std::unordered_set<char>{'#'});
  try{
    parse_fields();
  } catch (const stl_ios_utilities::BaseException& e) {
    if (GetParam().expectations["masked"].throws) {
      EXPECT_EQ(std::string(e.what()),
          GetParam().expectations["masked"].message);
    } else {
      throw;
    }
  }
  EXPECT_EQ(parsed_fields, GetParam().expectations["masked"].fields);
}

TEST_P(FieldParserParserFieldsTest, EnforceFieldNumberThrow) {
  try {
    parse_fields();
  } catch (const stl_ios_utilities::BaseException& e) {
    if (GetParam().expectations["enforce_field_number_throw"].throws) {
      EXPECT_EQ(std::string(e.what()),
          GetParam().expectations["enforce_field_number_throw"].message);
    } else {
      throw;
    }
  }
  EXPECT_EQ(parsed_fields,
      GetParam().expectations["enforce_field_number_throw"].fields);
}

TEST_P(FieldParserParserFieldsTest, EnforceFieldNumberNoThrow) {
  parser.enforce_field_number(false);
  try{
    parse_fields();
  } catch (const stl_ios_utilities::BaseException& e) {
    if (GetParam().expectations["enforce_field_number_no_throw"].throws) {
      EXPECT_EQ(std::string(e.what()),
          GetParam().expectations["enforce_field_number_no_throw"].message);
    } else {
      throw;
    }
  }
  EXPECT_EQ(parsed_fields,
      GetParam().expectations["enforce_field_number_no_throw"].fields);
}

TEST_P(FieldParserParserFieldsTest, IgnoreUnderfullData) {
  parser.enforce_field_number(false);
  parser.ignore_underfull_data(false);
  try{
    parse_fields();
  } catch (const stl_ios_utilities::BaseException& e) {
    if (GetParam().expectations["ignore_underfull_data"].throws) {
      EXPECT_EQ(std::string(e.what()),
          GetParam().expectations["ignore_underfull_data"].message);
    } else {
      throw;
    }
  }
  EXPECT_EQ(parsed_fields,
      GetParam().expectations["ignore_underfull_data"].fields);
}

TEST_P(FieldParserParserFieldsTest, FieldParsers) {
  std::unordered_map<int, std::function<void(std::string*)>> field_parsers{
    {1, [](std::string* s){s->append("-TEST");}},
  };
  parser.field_parsers(field_parsers);
  try{
    parse_fields();
  } catch (const stl_ios_utilities::BaseException& e) {
    if (GetParam().expectations["field_parsers"].throws) {
      EXPECT_EQ(std::string(e.what()),
          GetParam().expectations["field_parsers"].message);
    } else {
      throw;
    }
  }
  EXPECT_EQ(parse_fields, GetParam().expectations["field_parsers"].fields);
}

INSTANTIATE_TEST_SUITE_P(Expected,
                         FieldParserParserFieldsTest,
                         testing::ValuesIn(kTestCases));

} // namespace

} // namespace stl_ios_utilities