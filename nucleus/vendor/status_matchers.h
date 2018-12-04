/*
 * Copyright 2018 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef THIRD_PARTY_NUCLEUS_VENDOR_STATUS_MATCHERS_H_
#define THIRD_PARTY_NUCLEUS_VENDOR_STATUS_MATCHERS_H_

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "nucleus/vendor/statusor.h"

namespace nucleus {

// Matches if a Status or StatusOr object's ok() returns true.
MATCHER(IsOK, "") {
  return arg.ok();
}

// Matches if a Status or StatusOr object's ok() returns false.
MATCHER(IsNotOK, "") {
  return !arg.ok();
}

// Matches if a Status or StatusOr object's ok() returns false and that status
// code is expected_code.
MATCHER_P(IsNotOKWithCode, expected_code, "") {
  return (!arg.ok()) && arg.code() == expected_code;
}

// Matches if a Status or StatusOr object's ok() returns false and that status
// has an error message containing the string expected_error_message_substring.
MATCHER_P(IsNotOKWithMessage, expected_error_message_substring, "") {
  return (!arg.ok()) &&
         arg.error_message().find(expected_error_message_substring) !=
             std::string::npos;
}

// Matches if a Status or StatusOr object's ok() returns false and that status
// code is expected_code and its error message contains the string
// expected_error_message_substring.
MATCHER_P2(IsNotOKWithCodeAndMessage, expected_code,
           expected_error_message_substring, "") {
  return (!arg.ok()) && arg.code() == expected_code &&
         arg.error_message().find(expected_error_message_substring) !=
             std::string::npos;
}

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_VENDOR_STATUS_MATCHERS_H_
