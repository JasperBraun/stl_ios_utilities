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

#ifndef STL_IOS_UTILITIES_FIELD_PARSER_TEST_H_
#define STL_IOS_UTILITIES_FIELD_PARSER_TEST_H_

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "exceptions.h"

namespace stl_ios_utilities {

namespace test {

enum class TestPattern {
  // default parameters.
  kDefault,
  // reads 2 fields at a time and adds '_' to delimiters.
  kDelimiters,
  // reads 2 fields at a time and adds '#' to terminators.
  kTerminators,
  // reads 2 fields at a time and adds '#' to masked characters.
  kMasked,
  // reads 2 fields at a time.
  kEnforceFieldNumberThrow,
  // reads 2 fields at a time without field number enforcement.
  kEnforceFieldNumberNoThrow,
  // reads 2 fields at a time without field number enforcement and without
  // ignoring data read while more fields than present were expected.
  kIgnoreUnderfullData,
  // reads 2 fields at a time and appends "-TEST" to the first field.
  kFieldParsers};

// expected outcome of a test pattern
struct TestPatternExpectations {
  std::vector<std::vector<std::string>> fields;
  bool throws;
  BaseException exception;

  TestPatternExpectations(
      std::vector<std::vector<std::string>>&& expected_fields,
      bool throws_exception,
      BaseException&& expected_exception)
      : fields{expected_fields},
        throws{throws_exception},
        exception{expected_exception} {}
};

// input and expected outcomes of test patterns
struct TestValues {
  std::string input_string;
  std::unordered_map<int, TestPatternExpectations> expectations;

  TestValues(
      std::string&& input,
      std::unordered_map<int, TestPatternExpectations>&&
          pattern_expectations)
      : input_string{input}, expectations{pattern_expectations} {}
};

// exception that is never thrown; used as dummy when no exception is expected
struct NoThrowException final : public BaseException {
  using BaseException::BaseException;
};

std::vector<TestValues> kTestCases{
    TestValues(
        "foo_bar\tbaz_bum\tbel_bol\tr#f_h#d\n"
        "pif_paf\t#ic_fi#\ttin_tin\tti#_#do\n"
        "sin_cos\tt#c_f#p\trif_raf",
        std::unordered_map<int, TestPatternExpectations>{{
            {static_cast<int>(TestPattern::kDefault), TestPatternExpectations(
                {{"foo_bar"}, {"baz_bum"}, {"bel_bol"}, {"r#f_h#d"},
                 {"pif_paf"}, {"#ic_fi#"}, {"tin_tin"}, {"ti#_#do"},
                 {"sin_cos"}, {"t#c_f#p"}, {"rif_raf"}},
                false,
                NoThrowException("Unexpected exception.")
            )}, {static_cast<int>(TestPattern::kDelimiters), TestPatternExpectations(
                {{"foo", "bar"}, {"baz", "bum"}, {"bel", "bol"}, {"r#f", "h#d"},
                 {"pif", "paf"}, {"#ic", "fi#"}, {"tin", "tin"}, {"ti#", "#do"},
                 {"sin", "cos"}, {"t#c", "f#p"}, {"rif", "raf"}},
                false,
                NoThrowException("Unexpected exception.")
            )}, {static_cast<int>(TestPattern::kTerminators), TestPatternExpectations(
                {{"foo_bar", "baz_bum"}, {"bel_bol", "r"}},
                true,
                MissingFields("Too many fields requested by"
                              " `stl_ios_utilities::FieldParser::parse_row`.")
            )}, {static_cast<int>(TestPattern::kMasked), TestPatternExpectations(
                {{"foo_bar", "baz_bum"}, {"bel_bol", "rf_hd"},
                 {"pif_paf", "ic_fi"}, {"tin_tin", "ti_do"},
                 {"sin_cos", "tc_fp"}},
                true,
                MissingFields("Too many fields requested by"
                              " `stl_ios_utilities::FieldParser::parse_row`.")
            )}, {static_cast<int>(TestPattern::kEnforceFieldNumberThrow), TestPatternExpectations(
                {{"foo_bar", "baz_bum"}, {"bel_bol", "r#f_h#d"},
                 {"pif_paf", "#ic_fi#"}, {"tin_tin", "ti#_#do"},
                 {"sin_cos", "t#c_f#p"}},
                true,
                MissingFields("Too many fields requested by"
                              " `stl_ios_utilities::FieldParser::parse_row`.")
            )}, {static_cast<int>(TestPattern::kEnforceFieldNumberNoThrow), TestPatternExpectations(
                {{"foo_bar", "baz_bum"}, {"bel_bol", "r#f_h#d"},
                 {"pif_paf", "#ic_fi#"}, {"tin_tin", "ti#_#do"},
                 {"sin_cos", "t#c_f#p"}},
                false,
                NoThrowException("Unexpected exception.")
            )}, {static_cast<int>(TestPattern::kIgnoreUnderfullData), TestPatternExpectations(
                {{"foo_bar", "baz_bum"}, {"bel_bol", "r#f_h#d"},
                 {"pif_paf", "#ic_fi#"}, {"tin_tin", "ti#_#do"},
                 {"sin_cos", "t#c_f#p"}, {"rif_raf"}},
                false,
                NoThrowException("Unexpected exception.")
            )}, {static_cast<int>(TestPattern::kFieldParsers), TestPatternExpectations(
                {{"foo_bar-TEST", "baz_bum"}, {"bel_bol-TEST", "r#f_h#d"},
                 {"pif_paf-TEST", "#ic_fi#"}, {"tin_tin-TEST", "ti#_#do"},
                 {"sin_cos-TEST", "t#c_f#p"}},
                true,
                MissingFields("Too many fields requested by"
                              " `stl_ios_utilities::FieldParser::parse_row`.")
            )}
        }}
    )
};

} // namespace test

} // namespace stl_ios_utilities

#endif