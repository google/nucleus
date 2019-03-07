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
"""Tests for BedWriter CLIF python wrappers."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import gfile
from nucleus.io import tfrecord
from nucleus.io.python import bed_writer
from nucleus.protos import bed_pb2
from nucleus.testing import test_utils


_DOUBLE_CLOSE_ERROR = 'Cannot close an already closed BedWriter'
_WRITE_TO_CLOSED_ERROR = 'Cannot write to closed BED stream'


class WrapBedWriterTest(parameterized.TestCase):

  def setUp(self):
    out_fname = test_utils.test_tmpfile('output.bed')
    self.writer = bed_writer.BedWriter.to_file(
        out_fname, bed_pb2.BedHeader(num_fields=12), bed_pb2.BedWriterOptions())
    self.expected_bed_content = [
        'chr1\t10\t20\tfirst\t100\t+\t12\t18\t255,124,1\t3\t2,6,2\t10,12,18\n',
        'chr1\t100\t200\tsecond\t250\t.\t120\t180\t252,122,12\t2\t35,40\t'
        '100,160\n'
    ]
    self.record = bed_pb2.BedRecord(
        reference_name='chr1', start=20, end=30, name='r')

  def test_writing_canned_records(self):
    """Tests writing all the records that are 'canned' in our tfrecord file."""
    # This file is in TFRecord format.
    tfrecord_file = test_utils.genomics_core_testdata(
        'test_regions.bed.tfrecord')

    header = bed_pb2.BedHeader(num_fields=12)
    writer_options = bed_pb2.BedWriterOptions()
    bed_records = list(
        tfrecord.read_tfrecords(tfrecord_file, proto=bed_pb2.BedRecord))
    out_fname = test_utils.test_tmpfile('output.bed')
    with bed_writer.BedWriter.to_file(out_fname, header,
                                      writer_options) as writer:
      for record in bed_records:
        writer.write(record)

    with gfile.Open(out_fname, 'r') as f:
      self.assertEqual(f.readlines(), self.expected_bed_content)

  def test_context_manager(self):
    with self.writer:
      # Writing within the context manager succeeds.
      self.assertIsNone(self.writer.write(self.record))

    # self.writer should be closed, so writing again will fail.
    with self.assertRaisesRegexp(ValueError, _WRITE_TO_CLOSED_ERROR):
      self.writer.write(self.record)

  def test_double_context_manager(self):
    with self.writer:
      # Writing within the context manager succeeds.
      self.assertIsNone(self.writer.write(self.record))

    with self.assertRaisesRegexp(ValueError, _DOUBLE_CLOSE_ERROR):
      # Entering the closed writer should be fine.
      with self.writer:
        pass  # We want to raise an error on exit, so nothing to do in context.


if __name__ == '__main__':
  absltest.main()
