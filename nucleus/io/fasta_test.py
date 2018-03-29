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

"""Tests for nucleus.io.fasta."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import fasta
from nucleus.testing import test_utils
from nucleus.util import ranges


class RefFastaReaderTests(parameterized.TestCase):

  @parameterized.parameters('test.fasta', 'test.fasta.gz')
  def test_make_ref_reader_default(self, fasta_filename):
    fasta_path = test_utils.genomics_core_testdata(fasta_filename)
    with fasta.RefFastaReader(fasta_path) as reader:
      self.assertEqual(reader.query(ranges.make_range('chrM', 1, 6)), 'ATCAC')

  @parameterized.parameters('test.fasta', 'test.fasta.gz')
  def test_make_ref_reader_cache_specified(self, fasta_filename):
    fasta_path = test_utils.genomics_core_testdata(fasta_filename)
    with fasta.RefFastaReader(fasta_path, cache_size=10) as reader:
      self.assertEqual(reader.query(ranges.make_range('chrM', 1, 5)), 'ATCA')


if __name__ == '__main__':
  absltest.main()
