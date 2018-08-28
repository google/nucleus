# Copyright 2018 Google Inc.
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
"""Tests for GenomeReference CLIF python wrappers."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io.python import indexed_fasta_reader
from nucleus.testing import test_utils
from nucleus.util import ranges


class WrapReferenceTest(parameterized.TestCase):

  @parameterized.parameters('test.fasta', 'test.fasta.gz')
  def test_wrap(self, fasta_filename):
    chr_names = ['chrM', 'chr1', 'chr2']
    chr_lengths = [100, 76, 121]
    fasta = test_utils.genomics_core_testdata(fasta_filename)
    fai = test_utils.genomics_core_testdata(fasta_filename + '.fai')
    with indexed_fasta_reader.IndexedFastaReader.from_file(fasta, fai) as ref:
      self.assertEqual(ref.contig_names, chr_names)
      self.assertEqual(ref.bases(ranges.make_range('chrM', 1, 10)), 'ATCACAGGT')

      self.assertTrue(ref.is_valid_interval(ranges.make_range('chrM', 1, 10)))
      self.assertFalse(
          ref.is_valid_interval(ranges.make_range('chrM', 1, 100000)))

      self.assertEqual(len(ref.contigs), 3)
      self.assertEqual([c.name for c in ref.contigs], chr_names)
      self.assertEqual([c.n_bases for c in ref.contigs], chr_lengths)
      for contig in ref.contigs:
        self.assertEqual(ref.contig(contig.name), contig)
        self.assertTrue(ref.has_contig(contig.name))
        self.assertFalse(ref.has_contig(contig.name + '.unknown'))

  @parameterized.parameters(
      # The fasta and the FAI are both missing.
      ('missing.fasta', 'missing.fasta.fai'),
      # The fasta is present but the FAI is missing.
      ('test.fasta', 'missing.fasta.fai'),
      # The fasta is missing but the FAI is present.
      ('missing.fasta', 'test.fasta.fai'),
  )
  def test_from_file_raises_with_missing_inputs(self, fasta_filename,
                                                fai_filename):
    fasta = test_utils.genomics_core_testdata(fasta_filename)
    fai = test_utils.genomics_core_testdata(fai_filename)
    with self.assertRaisesRegexp(
        ValueError,
        'Not found: could not load fasta and/or fai for fasta ' + fasta):
      indexed_fasta_reader.IndexedFastaReader.from_file(fasta, fai)


if __name__ == '__main__':
  absltest.main()
