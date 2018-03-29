# Copyright 2018 Google Inc.
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
"""CLIF postprocessors."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


def ValueErrorOnFalse(ok, *args):
  """Returns None / arg / (args,...) if ok."""
  if not isinstance(ok, bool):
    raise TypeError('Use ValueErrorOnFalse only on bool return value')
  if not ok:
    raise ValueError('CLIF wrapped call returned False')
  # Plain return args will turn 1 into (1,)  and None into () which is unwanted.
  if args:
    return args if len(args) > 1 else args[0]
  return None


class WrappedCppIterable(object):
  """This class gives Python iteration semantics on top of a C++ 'Iterable'."""

  def __init__(self, cc_iterable):
    self._cc_iterable = cc_iterable

  def __enter__(self):
    self._cc_iterable.__enter__()
    return self

  def __exit__(self, type_, value, traceback):
    self._cc_iterable.__exit__(type_, value, traceback)

  def __iter__(self):
    return self

  def next(self):
    not_done, record = self._cc_iterable.Next()
    if not_done:
      return record
    else:
      raise StopIteration
