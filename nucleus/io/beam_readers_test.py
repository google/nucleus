# Copyright 2020 Google LLC.
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
"""Tests for nucleus.io.beam_readers."""

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import beam_readers
from nucleus.protos import reads_pb2
from nucleus.testing import test_utils

# pylint:disable=g-bad-import-order
# Beam needs to be imported after nucleus for this to work in open source.
from apache_beam.testing.test_pipeline import TestPipeline
from apache_beam.testing import util as beam_testing_util
# pylint:enable=g-bad-import-order


class TestReadBed(parameterized.TestCase):
  """Tests for reading BED files using Beam."""

  def _records_match(self, expected):
    def _equal(records):
      self.assertLen(records, len(expected))
      self.assertEqual([(r.reference_name, r.start, r.end) for r in records],
                       expected)
    return _equal

  def _len_match(self, expected):
    def _equal(records):
      self.assertLen(records, expected)
    return _equal

  @parameterized.parameters('test_regions.bed', 'test_regions.bed.gz',
                            'test_regions.bed.tfrecord',
                            'test_regions.bed.tfrecord.gz')
  def test_iterate_bed_reader(self, bed_filename):
    bed_path = test_utils.genomics_core_testdata(bed_filename)
    expected = [('chr1', 10, 20), ('chr1', 100, 200)]
    bed_path = test_utils.genomics_core_testdata(bed_filename)
    with TestPipeline() as p:
      records = (p | beam_readers.ReadBed(bed_path))
      beam_testing_util.assert_that(records, self._records_match(expected))

  def test_process_single_file_with_num_fields(self):
    # This BED file has 8 columns, but we will only read in three.
    bed_path = test_utils.genomics_core_testdata('test_regions.bed')
    with TestPipeline() as p:
      records = (p | beam_readers.ReadBed(bed_path, num_fields=3))
      beam_testing_util.assert_that(records, self._len_match(2))

  @parameterized.parameters(1, 2, 7, 10, 11, 13)
  def test_invalid_num_fields(self, invalid_num_fields):
    bed_path = test_utils.genomics_core_testdata('test_regions.bed')
    with self.assertRaisesRegexp(ValueError, 'Invalid requested number of fie'):
      with TestPipeline() as p:
        _ = (p | beam_readers.ReadBed(bed_path, num_fields=invalid_num_fields))

  def test_read_multiple_files(self):
    bed_path = test_utils.genomics_core_testdata('test.bed*')
    # test.bed and test.bed.gz each have four records.
    with TestPipeline() as p:
      records = (p | beam_readers.ReadBed(bed_path))
      beam_testing_util.assert_that(
          records, self._len_match(8))


class TestReadSam(parameterized.TestCase):
  """Tests for reading SAM/BAM files using Beam."""

  def _len_match(self, expected):
    def _equal(records):
      self.assertLen(records, expected)
    return _equal

  @parameterized.parameters('test.sam', 'test.sam.golden.tfrecord')
  def test_sam_iterate(self, sam_filename):
    with TestPipeline() as p:
      records = (
          p
          | beam_readers.ReadSam(
              test_utils.genomics_core_testdata(sam_filename)))
      beam_testing_util.assert_that(records, self._len_match(6))

  def test_bam_iterate(self):
    with TestPipeline() as p:
      records = (
          p
          | beam_readers.ReadSam(test_utils.genomics_core_testdata('test.bam')))
      beam_testing_util.assert_that(records, self._len_match(106))

  @parameterized.parameters(True, False)
  def test_read_multiple_files(self, keep_unaligned):
    file_pattern = test_utils.genomics_core_testdata('test.*am')
    read_requirements = reads_pb2.ReadRequirements(
        keep_unaligned=keep_unaligned)
    # test.sam contains 6 records and test.bam has 106 records. Both files
    # contain 1 unmapped record.
    expected_count = 112 if keep_unaligned else 110
    with TestPipeline() as p:
      result = (
          p
          | beam_readers.ReadSam(
              file_pattern, read_requirements=read_requirements))
      beam_testing_util.assert_that(result, self._len_match(expected_count))


if __name__ == '__main__':
  absltest.main()
