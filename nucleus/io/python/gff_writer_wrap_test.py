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
"""Tests for GffWriter CLIF python wrappers."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import tfrecord
from nucleus.io.python import gff_writer
from nucleus.protos import gff_pb2
from nucleus.testing import test_utils
from nucleus.util import ranges

_DOUBLE_CLOSE_ERROR = 'Cannot close an already closed GffWriter'
_WRITE_TO_CLOSED_ERROR = 'Cannot write to closed GFF stream'


class WrapGffWriterTest(parameterized.TestCase):

  def setUp(self):
    out_fname = test_utils.test_tmpfile('output.gff')
    self.writer = gff_writer.GffWriter.to_file(out_fname, gff_pb2.GffHeader(),
                                               gff_pb2.GffWriterOptions())
    self.expected_gff_content = open(
        test_utils.genomics_core_testdata('test_features.gff')).readlines()
    self.header = gff_pb2.GffHeader(
        sequence_regions=[ranges.make_range('ctg123', 0, 1497228)])
    self.record = gff_pb2.GffRecord(
        range=ranges.make_range('ctg123', 1000, 1100))

  def test_writing_canned_records(self):
    """Tests writing all the records that are 'canned' in our tfrecord file."""
    # This file is in TFRecord format.
    tfrecord_file = test_utils.genomics_core_testdata(
        'test_features.gff.tfrecord')
    writer_options = gff_pb2.GffWriterOptions()
    gff_records = list(
        tfrecord.read_tfrecords(tfrecord_file, proto=gff_pb2.GffRecord))
    out_fname = test_utils.test_tmpfile('output.gff')
    with gff_writer.GffWriter.to_file(out_fname, self.header,
                                      writer_options) as writer:
      for record in gff_records:
        writer.write(record)

    with open(out_fname) as f:
      self.assertEqual(f.readlines(), self.expected_gff_content)

  def test_context_manager(self):
    with self.writer:
      # Writing within the context manager succeeds.
      self.assertIsNone(self.writer.write(self.record))

    # self.writer should be closed, so writing again will fail.
    with self.assertRaisesRegexp(ValueError, _WRITE_TO_CLOSED_ERROR):
      self.writer.write(self.record)

  def test_double_close(self):
    with self.writer:
      # Writing within the context manager succeeds.
      self.assertIsNone(self.writer.write(self.record))

    with self.assertRaisesRegexp(ValueError, _DOUBLE_CLOSE_ERROR):
      # Entering the closed writer should be fine.
      with self.writer:
        pass  # We want to raise an error on exit, so nothing to do in context.


if __name__ == '__main__':
  absltest.main()
