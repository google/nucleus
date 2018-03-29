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
"""Tests for nucleus.io.fastq."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import fastq
from nucleus.protos import fastq_pb2
from nucleus.testing import test_utils


class FastqReaderTests(parameterized.TestCase):

  @parameterized.parameters('test_reads.fastq', 'test_reads.fastq.gz',
                            'test_reads.tfrecord', 'test_reads.tfrecord.gz')
  def test_iterate_fastq_reader(self, fastq_filename):
    fastq_path = test_utils.genomics_core_testdata(fastq_filename)
    expected_ids = [
        'NODESC:header', 'M01321:49:000000000-A6HWP:1:1101:17009:2216', 'FASTQ'
    ]
    with fastq.FastqReader(fastq_path) as reader:
      records = list(reader.iterate())
    self.assertLen(records, 3)
    self.assertEqual([r.id for r in records], expected_ids)


class FastqWriterTests(parameterized.TestCase):
  """Tests for FastqWriter."""

  def setUp(self):
    self.records = [
        fastq_pb2.FastqRecord(id='id1', sequence='ACGTG', quality='ABCDE'),
        fastq_pb2.FastqRecord(id='id2', sequence='ATTT', quality='ABC@'),
        fastq_pb2.FastqRecord(
            id='ID3',
            description='multi desc',
            sequence='GATAC',
            quality='ABC@!'),
    ]

  @parameterized.parameters('test_raw.fastq', 'test_zipped.fastq.gz',
                            'test_raw.tfrecord', 'test_zipped.tfrecord.gz')
  def test_roundtrip_writer(self, filename):
    output_path = test_utils.test_tmpfile(filename)
    with fastq.FastqWriter(output_path) as writer:
      for record in self.records:
        writer.write(record)

    with fastq.FastqReader(output_path) as reader:
      v2_records = list(reader.iterate())

    self.assertEqual(self.records, v2_records)


if __name__ == '__main__':
  absltest.main()
