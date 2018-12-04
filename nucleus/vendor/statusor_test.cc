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

#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/platform/test.h"
#include "nucleus/vendor/status_matchers.h"
#include "tensorflow/core/lib/core/status.h"

namespace nucleus {

using tensorflow::Status;

// Status tests.
TEST(StatusTest, OKStatusMatchesIsOK) {
  EXPECT_THAT(Status::OK(), IsOK());
}

TEST(StatusTest, FailedStatusMatchesIsNotOK) {
  EXPECT_THAT(tensorflow::errors::Unknown("fail"), IsNotOK());
}

TEST(StatusTest, FailedStatusMatchesIsNotOKCode) {
  EXPECT_THAT(tensorflow::errors::Unknown("fail"),
              IsNotOKWithCode(tensorflow::error::UNKNOWN));
}

TEST(StatusTest, FailedStatusMatchesIsNotOKWithMessage) {
  EXPECT_THAT(tensorflow::errors::Unknown("fail"), IsNotOKWithMessage("fail"));
}

TEST(StatusTest, FailedStatusMatchesIsNotOKWithCodeAndMessage) {
  EXPECT_THAT(tensorflow::errors::Unknown("fail"),
              IsNotOKWithCodeAndMessage(tensorflow::error::UNKNOWN, "fail"));
}

// StatusOr tests.
TEST(StatusOrTest, OKStatusMatchesIsOK) {
  StatusOr<int> status_or = StatusOr<int>(0);
  EXPECT_THAT(status_or, IsOK());
}

TEST(StatusOrTest, FailedStatusMatchesIsNotOK) {
  StatusOr<int> status_or = tensorflow::errors::Unknown("fail");
  EXPECT_THAT(status_or, IsNotOK());
}

TEST(StatusOrTest, FailedStatusMatchesIsNotOKCode) {
  StatusOr<int> status_or = tensorflow::errors::Unknown("fail");
  EXPECT_THAT(status_or, IsNotOKWithCode(tensorflow::error::UNKNOWN));
}

TEST(StatusOrTest, FailedStatusMatchesIsNotOKWithMessage) {
  StatusOr<int> status_or = tensorflow::errors::Unknown("fail");
  EXPECT_THAT(status_or, IsNotOKWithMessage("fail"));
}

TEST(StatusOrTest, FailedStatusMatchesIsNotOKWithCodeAndMessage) {
  StatusOr<int> status_or = tensorflow::errors::Unknown("fail");
  EXPECT_THAT(status_or,
              IsNotOKWithCodeAndMessage(tensorflow::error::UNKNOWN, "fail"));
}

}  // namespace nucleus
