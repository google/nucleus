/*
 * Copyright 2018 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// Testing C++ utilities.
#ifndef THIRD_PARTY_NUCLEUS_TESTING_TEST_UTILS_H_
#define THIRD_PARTY_NUCLEUS_TESTING_TEST_UTILS_H_

#include <gmock/gmock-generated-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include "tensorflow/core/platform/test.h"
#include "absl/strings/string_view.h"
#include "nucleus/io/reader_base.h"
#include "nucleus/protos/reads.pb.h"
#include "nucleus/protos/reference.pb.h"
#include "nucleus/vendor/statusor.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/lib/io/record_reader.h"
#include "tensorflow/core/lib/io/record_writer.h"
#include "nucleus/platform/types.h"

namespace nucleus {


constexpr char kBioTFCoreTestDataDir[] = "nucleus/testdata";

// N.B. kDefaultWorkspace will be set to "" in OSS.
constexpr char kDefaultWorkspace[] = "";


// Simple getter for test files in the right testdata path.
// This uses JoinPath, so no leading or trailing "/" are necessary.
string GetTestData(
    absl::string_view path,
    absl::string_view test_data_dir = kBioTFCoreTestDataDir);

// Returns a path to a temporary file with filename in the appropriate test
// directory.
string MakeTempFile(absl::string_view filename);

// Reads all of the records from path into a vector of parsed Proto. Path
// must point to a TFRecord formatted file.
template <typename Proto>
std::vector<Proto> ReadProtosFromTFRecord(const string& path) {
  tensorflow::Env* env = tensorflow::Env::Default();
  std::unique_ptr<tensorflow::RandomAccessFile> read_file;
  TF_CHECK_OK(env->NewRandomAccessFile(path, &read_file));
  tensorflow::io::RecordReader reader(read_file.get());
  std::vector<Proto> results;
  tensorflow::uint64 offset = 0;
  tensorflow::tstring data;
  while (reader.ReadRecord(&offset, &data).ok()) {
    Proto proto;
    CHECK(proto.ParseFromArray(data.data(), data.size()))
        << "Failed to parse proto";
    results.push_back(proto);
  }
  return results;
}

// Writes all `protos` to a TFRecord formatted file.
template <typename Proto>
void WriteProtosToTFRecord(const std::vector<Proto>& protos,
                           const string& output_path) {
  std::unique_ptr<tensorflow::WritableFile> file;
  TF_CHECK_OK(tensorflow::Env::Default()->NewWritableFile(output_path, &file));
  tensorflow::io::RecordWriter record_writer(file.get());
  for (const Proto& proto : protos) {
    TF_CHECK_OK(record_writer.WriteRecord(proto.SerializeAsString()));
  }
}

// Creates a vector of ContigInfos with specified `names` and `positions`
// representing pos_in_fast. `names` and `positions` need to have the same
// number of elements.
std::vector<nucleus::genomics::v1::ContigInfo> CreateContigInfos(
    const std::vector<string>& names, const std::vector<int>& positions);

// A matcher to help us do Pointwise double expectations with a provided
// precision via DoubleNear.
MATCHER_P(PointwiseDoubleNear, abs_error, "") {
  using RhsType = decltype(::testing::get<1>(arg));
  ::testing::Matcher<RhsType> matcher =
      ::testing::DoubleNear(::testing::get<1>(arg), abs_error);
  return matcher.Matches(::testing::get<0>(arg));
}

// A matcher to test if a floating point value is finite.
MATCHER(IsFinite, "") { return std::isfinite(arg); }

// Adapter to extract an iterable into a vector for examination in test code
// from a StatusOr<std::shared_ptr<Iterable<Record>>>.
template <class Record>
std::vector<Record> as_vector(
    const StatusOr<std::shared_ptr<Iterable<Record>>>& it) {
  TF_CHECK_OK(it.status());
  return as_vector(it.ValueOrDie());
}

// Adapter to extract an iterable into a vector for examination in test code.
template<class Record>
std::vector<Record> as_vector(const std::shared_ptr<Iterable<Record>>& it) {
  std::vector<Record> records;
  for (const StatusOr<Record*> value_status : it) {
    records.push_back(*value_status.ValueOrDie());
  }
  return records;
}

// Creates a test Read.
//
// The read has reference_name chr, start of start, aligned_sequence of bases,
// and cigar element parsed from cigar_elements, which is vector of standard
// CIGAR element string values like {"5M", "2I", "3M"} which is 5 bp matches,
// 2 bp insertion, and 3 bp matches. The read has base qualities set to 30 and
// a mapping quality of 90.
::nucleus::genomics::v1::Read MakeRead(
    const string& chr, int start, const string& bases,
    const std::vector<string>& cigar_elements);

// Creates a test Cigar as a vector of CigarUnit.
//
// The Cigar elements are parsed from cigar_elements, which is vector of
// standard CIGAR element string values like {"5M", "2I", "3M"} which is 5 bp
// matches, 2 bp insertion, and 3 bp matches.
std::vector<::nucleus::genomics::v1::CigarUnit> MakeCigar(
    const std::vector<std::string>& cigar_elements);

// Determines whether file content represents GZIP'd data, based on file magic.
bool IsGzipped(absl::string_view file_contents);

}  // namespace nucleus


#endif  // THIRD_PARTY_NUCLEUS_TESTING_TEST_UTILS_H_
