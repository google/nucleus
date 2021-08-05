# Copyright 2018 Google LLC.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Tests for gff_reader CLIF python wrappers."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import clif_postproc
from nucleus.io.python import gff_reader
from nucleus.protos import gff_pb2
from nucleus.testing import test_utils


class GffReaderTest(parameterized.TestCase):

  def setUp(self):
    self.options = gff_pb2.GffReaderOptions()
    self.first = gff_pb2.GffRecord()
    self.first.range.reference_name = 'ctg123'
    self.first.range.start = 999
    self.first.range.end = 9000
    self.first.source = 'GenBank'
    self.first.type = 'gene'
    self.first.score = 2.5
    self.first.strand = gff_pb2.GffRecord.FORWARD_STRAND
    self.first.phase = 0
    self.first.attributes['ID'] = 'gene00001'
    self.first.attributes['Name'] = 'EDEN'

    self.second = gff_pb2.GffRecord()
    self.second.range.reference_name = 'ctg123'
    self.second.range.start = 999
    self.second.range.end = 1012
    self.second.phase = -1
    self.second.score = -float('inf')

  @parameterized.parameters('test_features.gff', 'test_features.gff.gz')
  def test_gff_iterate(self, test_features_gff_filename):
    file_path = test_utils.genomics_core_testdata(test_features_gff_filename)
    with gff_reader.GffReader.from_file(file_path, self.options) as reader:
      iterable = reader.iterate()
      self.assertIsInstance(iterable, clif_postproc.WrappedCppIterable)
      actual = list(iterable)
      self.assertLen(actual, 2)
      self.assertEqual(actual[0], self.first)
      self.assertEqual(actual[1], self.second)

  def test_from_file_raises_with_missing_gff(self):
    # TODO(b/194924033): Swap statement after error code string is updated
    # with self.assertRaisesRegexp(ValueError,
    #                              'NOT_FOUND: Could not open missing.gff'):
    with self.assertRaisesRegexp(ValueError,
                                 'Could not open missing.gff'):
      gff_reader.GffReader.from_file('missing.gff', self.options)

  def test_ops_on_closed_reader_raise(self):
    file_path = test_utils.genomics_core_testdata('test_features.gff')
    reader = gff_reader.GffReader.from_file(file_path, self.options)
    with reader:
      pass
    # At this point the reader is closed.
    with self.assertRaisesRegexp(ValueError, 'Cannot Iterate a closed'):
      reader.iterate()


if __name__ == '__main__':
  absltest.main()
