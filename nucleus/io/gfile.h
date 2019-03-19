/*
 * Copyright 2019 Google LLC.
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

#ifndef THIRD_PARTY_NUCLEUS_IO_GFILE_H_
#define THIRD_PARTY_NUCLEUS_IO_GFILE_H_

#include <string>
#include <vector>

namespace tensorflow {
class WritableFile;
namespace io {
class BufferedInputStream;
}  // namespace io
}  // namespace tensorflow

namespace nucleus {

// Return whether or not filename exists as a file.
bool Exists(const std::string& filename);

// Return all files matching the shell-style file glob.
std::vector<std::string> Glob(const std::string& pattern);

class ReadableFile {
 public:
  static ReadableFile* New(const std::string& filename);
  ~ReadableFile();

  // Reads the next line into *s, and returns true if that went ok.
  bool Readline(std::string* s);

  void Close();

  // This no-op function is needed only for Python context manager support.
  void PythonEnter() const {}

 private:
  ReadableFile();

  tensorflow::io::BufferedInputStream* stream_;  // Owned
};

class WritableFile {
 public:
  static WritableFile* New(const std::string& filename);
  ~WritableFile();

  bool Write(const std::string& s);
  void Close();

  // This no-op function is needed only for Python context manager support.
  void PythonEnter() const {}

 private:
  WritableFile();

  tensorflow::WritableFile* file_;  // Owned
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_IO_GFILE_H_
