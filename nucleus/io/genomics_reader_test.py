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
"""Tests for nucleus.io.vcf."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.io import genomics_reader
from tensorflow.python.lib.io import python_io


class DummyReader(genomics_reader.GenomicsReader):
  """A GenomicsReader that produces consecutive integers."""

  def __init__(self, input_path):
    self.limit = int(input_path)
    super(DummyReader, self).__init__()

  def iterate(self):
    for i in range(self.limit):
      yield i

  def query(self, region):
    raise NotImplementedError('Can not query DummyReader')

  def __exit__(self, exit_type, exit_value, exit_traceback):
    pass


class GenomicsReaderTests(absltest.TestCase):
  """Tests for GenomicsReader."""

  def testIteration(self):
    dreader = DummyReader('10')
    values = list(dreader)
    self.assertEqual(range(10), values)

  def testTwoIteratorsAtTheSameTime(self):
    dreader = DummyReader('100')
    iter2 = iter(dreader)
    for i in range(100):
      self.assertEqual(i, dreader.next())
      self.assertEqual(i, iter2.next())


class DummyProto(object):
  """A pretend protocol buffer class that only provides FromString."""

  def FromString(self, buf):
    return buf


class TFRecordReaderTests(absltest.TestCase):
  """Tests for TFRecordReader."""

  def setUp(self):
    python_io.tf_record_iterator = lambda x, y: x.split(',')

  def testMock(self):
    reader = genomics_reader.TFRecordReader('a,b,c,d,e', DummyProto())
    self.assertEqual(['a','b','c','d','e'], list(reader))

  def testTwoIteratorsAtTheSameTime(self):
    dreader = genomics_reader.TFRecordReader('0,1,2,3,4,5', DummyProto())
    iter2 = iter(dreader)
    for i in range(6):
      self.assertEqual(str(i), dreader.next())
      self.assertEqual(str(i), iter2.next())


if __name__ == '__main__':
  absltest.main()
