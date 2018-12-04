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
"""Tests for bed_reader CLIF python wrappers."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import clif_postproc
from nucleus.io.python import bed_reader
from nucleus.protos import bed_pb2
from nucleus.testing import test_utils


class BedReaderTest(parameterized.TestCase):

  def setUp(self):
    self.bed = test_utils.genomics_core_testdata('test_regions.bed')
    self.zipped_bed = test_utils.genomics_core_testdata('test_regions.bed.gz')
    self.options = bed_pb2.BedReaderOptions()
    self.first = bed_pb2.BedRecord(
        reference_name='chr1',
        start=10,
        end=20,
        name='first',
        score=100,
        strand=bed_pb2.BedRecord.FORWARD_STRAND,
        thick_start=12,
        thick_end=18,
        item_rgb='255,124,1',
        block_count=3,
        block_sizes='2,6,2',
        block_starts='10,12,18')

  def test_bed_iterate(self):
    with bed_reader.BedReader.from_file(self.bed, self.options) as reader:
      self.assertEqual(reader.header.num_fields, 12)
      iterable = reader.iterate()
      self.assertIsInstance(iterable, clif_postproc.WrappedCppIterable)
      actual = list(iterable)
      self.assertLen(actual, 2)
      self.assertEqual(actual[0], self.first)

    zreader = bed_reader.BedReader.from_file(
        self.zipped_bed,
        bed_pb2.BedReaderOptions())
    self.assertEqual(zreader.header.num_fields, 12)
    with zreader:
      ziterable = zreader.iterate()
      self.assertIsInstance(ziterable, clif_postproc.WrappedCppIterable)
      zactual = list(ziterable)
      self.assertLen(zactual, 2)
      self.assertEqual(zactual[0], self.first)

  def test_from_file_raises_with_missing_bed(self):
    with self.assertRaisesRegexp(ValueError,
                                 'Not found: Could not open missing.bed'):
      bed_reader.BedReader.from_file('missing.bed', self.options)

  def test_ops_on_closed_reader_raise(self):
    reader = bed_reader.BedReader.from_file(self.bed, self.options)
    with reader:
      pass
    # At this point the reader is closed.
    with self.assertRaisesRegexp(ValueError, 'Cannot Iterate a closed'):
      reader.iterate()

  @parameterized.parameters('malformed.bed', 'malformed2.bed')
  def test_bed_iterate_raises_on_malformed_record(self, filename):
    malformed = test_utils.genomics_core_testdata(filename)
    reader = bed_reader.BedReader.from_file(malformed, self.options)
    iterable = iter(reader.iterate())
    self.assertIsNotNone(next(iterable))
    with self.assertRaises(ValueError):
      list(iterable)


if __name__ == '__main__':
  absltest.main()
