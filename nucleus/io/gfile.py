# Copyright 2018 Google LLC.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""A Python interface for files."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import six

from nucleus.io.python import gfile


def Exists(filename):
  return gfile.Exists(filename)


def Glob(pattern):
  return gfile.Glob(pattern)


class ReadableFile(six.Iterator):
  """Wraps gfile.ReadableFile to add iteration, enter/exit and readlines."""

  def __init__(self, filename):
    self._file = gfile.ReadableFile.New(filename)

  def __enter__(self):
    return self

  def __exit__(self, type_, value, traceback):
    self._file.__exit__()

  def __iter__(self):
    return self

  def __next__(self):
    ok, line = self._file.Readline()
    if ok:
      return line
    else:
      raise StopIteration

  def readlines(self):
    lines = []
    while True:
      ok, line = self._file.Readline()
      if ok:
        lines.append(line)
      else:
        break
    return lines


def Open(filename, mode="r"):
  if mode.startswith("r"):
    return ReadableFile(filename)
  elif mode.startswith("w"):
    return gfile.WritableFile.New(filename)
  else:
    raise ValueError("Unsupported mode '{}' for Open".format(mode))
