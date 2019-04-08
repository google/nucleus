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
"""CLIF postprocessors."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import abc
import six

from nucleus.protos import bed_pb2
from nucleus.protos import bedgraph_pb2
from nucleus.protos import fastq_pb2
from nucleus.protos import gff_pb2
from nucleus.protos import reads_pb2
from nucleus.protos import variants_pb2


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


class WrappedCppIterable(six.Iterator):
  """This class gives Python iteration semantics on top of a C++ 'Iterable'."""

  __metaclass__ = abc.ABCMeta

  def __init__(self, cc_iterable):
    self._cc_iterable = cc_iterable

  def __enter__(self):
    self._cc_iterable.__enter__()
    return self

  def __exit__(self, type_, value, traceback):
    self._cc_iterable.__exit__(type_, value, traceback)

  def __iter__(self):
    return self

  @abc.abstractmethod
  def _raw_next(self):
    """Sub-classes should implement __next__ in this method."""

  def __next__(self):
    try:
      record, not_done = self._raw_next()
    except AttributeError:
      if self._cc_iterable is None:
        raise ValueError('No underlying iterable. This may occur if trying to '
                         'create multiple concurrent iterators from the same '
                         'reader. Try wrapping your call to the iterator in a '
                         '`with` block or materializing the entire iterable '
                         'explicitly.')
      else:
        raise
    if not_done:
      return record
    else:
      raise StopIteration


class WrappedBedIterable(WrappedCppIterable):

  def _raw_next(self):
    record = bed_pb2.BedRecord()
    not_done = self._cc_iterable.PythonNext(record)
    return record, not_done


class WrappedBedGraphIterable(WrappedCppIterable):

  def _raw_next(self):
    record = bedgraph_pb2.BedGraphRecord()
    not_done = self._cc_iterable.PythonNext(record)
    return record, not_done


class WrappedFastqIterable(WrappedCppIterable):

  def _raw_next(self):
    record = fastq_pb2.FastqRecord()
    not_done = self._cc_iterable.PythonNext(record)
    return record, not_done


class WrappedGffIterable(WrappedCppIterable):

  def _raw_next(self):
    record = gff_pb2.GffRecord()
    not_done = self._cc_iterable.PythonNext(record)
    return record, not_done


class WrappedReferenceIterable(WrappedCppIterable):

  def _raw_next(self):
    not_done, record = self._cc_iterable.Next()
    return record, not_done


class WrappedSamIterable(WrappedCppIterable):

  def _raw_next(self):
    record = reads_pb2.Read()
    not_done = self._cc_iterable.PythonNext(record)
    return record, not_done


class WrappedVariantIterable(WrappedCppIterable):

  def _raw_next(self):
    record = variants_pb2.Variant()
    not_done = self._cc_iterable.PythonNext(record)
    return record, not_done
