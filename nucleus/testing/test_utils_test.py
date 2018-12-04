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
"""Tests for nucleus's testing.test_utils."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.protos import cigar_pb2
from nucleus.testing import test_utils


class TestUtilsTests(absltest.TestCase):

  def test_make_read(self):
    bases = 'ACG'
    quals = [30, 40, 50]
    cigar = '3M'
    mapq = 42
    chrom = 'chr10'
    start = 123
    name = 'myname'
    read = test_utils.make_read(
        bases,
        quals=quals,
        cigar=cigar,
        mapq=mapq,
        chrom=chrom,
        start=start,
        name=name)

    self.assertEqual(read.aligned_sequence, bases)
    self.assertEqual(read.aligned_quality, quals)
    self.assertEqual(list(read.alignment.cigar), [
        cigar_pb2.CigarUnit(
            operation_length=3, operation=cigar_pb2.CigarUnit.ALIGNMENT_MATCH)
    ])
    self.assertEqual(read.alignment.mapping_quality, mapq)
    self.assertEqual(read.alignment.position.reference_name, chrom)
    self.assertEqual(read.alignment.position.position, start)
    self.assertEqual(read.fragment_name, name)

  def test_make_read_produces_unique_read_names(self):
    read1 = test_utils.make_read('A')
    read2 = test_utils.make_read('A')
    self.assertGreater(len(read1.fragment_name), 0)
    self.assertGreater(len(read2.fragment_name), 0)
    self.assertNotEqual(read1.fragment_name, read2.fragment_name)


if __name__ == '__main__':
  absltest.main()
