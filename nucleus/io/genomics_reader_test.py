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
"""Tests for nucleus.io.vcf."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
import six

from nucleus.io import genomics_reader
from nucleus.protos import gff_pb2
from nucleus.testing import test_utils


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
    self.assertEqual(list(range(10)), list(dreader))

  def testTwoIteratorsAtTheSameTime(self):
    dreader = DummyReader('100')
    iter2 = iter(dreader)
    for i in range(100):
      self.assertEqual(i, six.next(dreader))
      self.assertEqual(i, six.next(iter2))


class TFRecordReaderTests(absltest.TestCase):
  """Tests for TFRecordReader."""

  def testUncompressed(self):
    reader = genomics_reader.TFRecordReader(
        test_utils.genomics_core_testdata('test_features.gff.tfrecord'),
        gff_pb2.GffRecord(),
    )
    records = list(reader.iterate())
    self.assertEqual('GenBank', records[0].source)
    self.assertEqual('ctg123', records[1].range.reference_name)

  def testUncompressedExplicit(self):
    reader = genomics_reader.TFRecordReader(
        test_utils.genomics_core_testdata('test_features.gff.tfrecord'),
        gff_pb2.GffRecord(),
        compression_type=''
    )
    records = list(reader.iterate())
    self.assertEqual('GenBank', records[0].source)
    self.assertEqual('ctg123', records[1].range.reference_name)

  def testCompressed(self):
    reader = genomics_reader.TFRecordReader(
        test_utils.genomics_core_testdata('test_features.gff.tfrecord.gz'),
        gff_pb2.GffRecord(),
    )
    records = list(reader.iterate())
    self.assertEqual('GenBank', records[0].source)
    self.assertEqual('ctg123', records[1].range.reference_name)

  def testCompressedExplicit(self):
    reader = genomics_reader.TFRecordReader(
        test_utils.genomics_core_testdata('test_features.gff.tfrecord.gz'),
        gff_pb2.GffRecord(),
        compression_type='GZIP'
    )
    records = list(reader.iterate())
    self.assertEqual('GenBank', records[0].source)
    self.assertEqual('ctg123', records[1].range.reference_name)


if __name__ == '__main__':
  absltest.main()
