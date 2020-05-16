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

#ifndef STL_IOS_UTILITIES_EXCEPTIONS_H_
#define STL_IOS_UTILITIES_EXCEPTIONS_H_

namespace stl_ios_utilities {

  /// @brief Base class for exceptions thrown by entities in the
  ///  `stl_ios_utilities` namespace.
  ///
  /// @details Whenever possible, an exception of a derived type is thrown. Some
  ///  entities in the `stl_ios_utilities` namespace however use STL functions
  ///  and operations which may throw their own exceptions. This is documented
  ///  for each entity separately.
  ///
  struct BaseException : std::logic_error {
    using std::logic_error::logic_error;
  }

  /// @brief Indicates that not enough field were read.
  ///
  /// @details Typically thrown when an operation expected to read more fields
  ///  then it was able to.
  ///
  struct MissingFields : BaseException {
    using BaseException::BaseException;
  };

  /// @brief Indicates that not enough field were read.
  ///
  /// @details Typically thrown when an operation encounters more fields then
  ///  expected.
  ///
  struct UnexpectedFields : BaseException {
    using BaseException::BaseException;
  };

} // namespace stl_ios_utilities

#endif // STL_IOS_UTILITIES_EXCEPTIONS_H_