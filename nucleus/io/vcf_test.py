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
"""Tests for nucleus.io.vcf."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import vcf
from nucleus.protos import reference_pb2
from nucleus.protos import struct_pb2
from nucleus.protos import variants_pb2
from nucleus.testing import test_utils
from nucleus.util import ranges
from tensorflow.python.platform import gfile


class VcfHeaderCacheTests(parameterized.TestCase):
  """Test the functionality of the VcfHeaderCache class."""

  def setUp(self):
    self.vcf_reader = vcf.VcfReader(
        test_utils.genomics_core_testdata('test_sites.vcf'), use_index=False)
    self.cache = self.vcf_reader.field_access_cache

  @parameterized.parameters(
      'DP',
      'AF',
      'END',
      'ExcessHet',
      'culprit',
  )
  def test_valid_info_get_funcs(self, field_name):
    fn = self.cache.info_field_get_fn(field_name)
    self.assertTrue(callable(fn))

  @parameterized.parameters(
      'DP',
      'AF',
      'END',
      'ExcessHet',
      'culprit',
      'HaplotypeScore',
      'InbreedingCoeff',
  )
  def test_valid_info_set_funcs(self, field_name):
    fn = self.cache.info_field_set_fn(field_name)
    self.assertTrue(callable(fn))

  def test_invalid_info_funcs(self):
    with self.assertRaises(KeyError):
      self.cache.info_field_get_fn('RGQ')
    with self.assertRaises(KeyError):
      self.cache.info_field_set_fn('PID')

  @parameterized.parameters(
      'AD',
      'DP',
      'PID',
      'RGQ',
  )
  def test_valid_format_get_funcs(self, field_name):
    fn = self.cache.format_field_get_fn(field_name)
    self.assertTrue(callable(fn))

  @parameterized.parameters(
      'AD',
      'DP',
      'PID',
      'RGQ',
  )
  def test_valid_format_set_funcs(self, field_name):
    fn = self.cache.format_field_set_fn(field_name)
    self.assertTrue(callable(fn))

  def test_invalid_format_funcs(self):
    with self.assertRaises(KeyError):
      self.cache.format_field_get_fn('culprit')
    with self.assertRaises(KeyError):
      self.cache.format_field_set_fn('ExcessHet')


class VcfReaderTests(absltest.TestCase):
  """Test the iteration functionality provided by vcf.VcfReader."""

  def setUp(self):
    self.sites_reader = vcf.VcfReader(
        test_utils.genomics_core_testdata('test_sites.vcf'), use_index=False)

    self.samples_reader = vcf.VcfReader(
        test_utils.genomics_core_testdata('test_samples.vcf.gz'),
        use_index=True)

  def test_vcf_iterate(self):
    self.assertEqual(test_utils.iterable_len(self.sites_reader.iterate()), 5)

  def test_vcf_query(self):
    range1 = ranges.parse_literal('chr3:100,000-500,000')
    self.assertEqual(
        test_utils.iterable_len(self.samples_reader.query(range1)), 4)

  def test_vcf_iter(self):
    n = 0
    for _ in self.sites_reader:
      n += 1
    self.assertEqual(n, 5)


def _format_expected_variant(ref, alts, format_spec, *samples):
  base = ['20', 1, '.', ref, alts, 0, '.', '.', format_spec]
  return base + list(samples)


def _format_test_variant(alleles, call_infos):
  variant = test_utils.make_variant(chrom='20', start=0, alleles=alleles)
  for i, call_info in enumerate(call_infos):
    call = variant.calls.add(call_set_name='sample' + str(i))
    for key, value in call_info.items():
      if not isinstance(value, (list, tuple)):
        value = [value]
      call.info[key].values.extend(
          [struct_pb2.Value(int_value=v) for v in value])
  return variant


class VcfWriterTests(parameterized.TestCase):
  """Tests for VcfWriter."""

  def assertWrittenVCFRecordsEqual(self, path, expected_lines):

    def cleanup_line(line):
      if isinstance(line, (list, tuple)):
        return '\t'.join(str(x) for x in line)
      else:
        return line

    expected_lines = [cleanup_line(line) for line in expected_lines]
    with gfile.FastGFile(path, 'r') as fin:
      self.assertEqual([
          line.strip() for line in fin.readlines() if not line.startswith('#')
      ], expected_lines)

  def write_variant_to_tempfile(self, variant):
    output_path = test_utils.test_tmpfile('test.vcf')
    header = variants_pb2.VcfHeader(
        contigs=[reference_pb2.ContigInfo(name='20')],
        sample_names=[call.call_set_name for call in variant.calls],
        formats=[
            variants_pb2.VcfFormatInfo(
                id='DP', number='1', type='Integer', description='Read depth'),
            variants_pb2.VcfFormatInfo(
                id='AD',
                number='R',
                type='Integer',
                description='Read depth for each allele')
        ])
    writer = vcf.VcfWriter(output_path, header=header)
    with writer:
      writer.write(variant)
    return output_path

  @parameterized.parameters(
      # Check that our DP field is getting written out properly.
      (_format_test_variant(['A', 'T'], [{
          'DP': 1
      }, {
          'DP': 2
      }]), _format_expected_variant('A', 'T', 'DP', '1', '2')),
      # Checks that we get the missing value when DP is missing in some samples.
      (_format_test_variant(['A', 'T'], [{
          'DP': 1
      }, {}]), _format_expected_variant('A', 'T', 'DP', '1', '.')),
      (_format_test_variant(['A', 'T'], [{}, {
          'DP': 2
      }]), _format_expected_variant('A', 'T', 'DP', '.', '2')),
  )
  def test_single_value_format_field(self, variant, expected_vcf_line):
    self.assertWrittenVCFRecordsEqual(
        self.write_variant_to_tempfile(variant), [expected_vcf_line])

  @parameterized.parameters(
      # Check that our AD field is getting written correctly.
      (_format_test_variant(['A', 'T'], [{
          'AD': [0, 1]
      }, {
          'AD': [2, 3]
      }]), _format_expected_variant('A', 'T', 'AD', '0,1', '2,3')),
      (_format_test_variant(['A', 'T'], [{}, {
          'AD': [2, 3]
      }]), _format_expected_variant('A', 'T', 'AD', '.', '2,3')),
      (_format_test_variant(['A', 'T'], [{
          'AD': [0, 1]
      }, {}]), _format_expected_variant('A', 'T', 'AD', '0,1', '.')),
      # Let's try a tri-allelic site where we have 3 AD values / sample.
      (_format_test_variant(['A', 'T', 'C'], [{
          'AD': [0, 1, 2]
      }, {
          'AD': [4, 5, 6]
      }]), _format_expected_variant('A', 'T,C', 'AD', '0,1,2', '4,5,6')),
      # Check that we handle missing values properly.
      (_format_test_variant(['A', 'T', 'C'], [{
          'AD': [0, 1, 2]
      }, {}]), _format_expected_variant('A', 'T,C', 'AD', '0,1,2', '.')),
      (_format_test_variant(['A', 'T', 'C'], [{}, {
          'AD': [4, 5, 6]
      }]), _format_expected_variant('A', 'T,C', 'AD', '.', '4,5,6')),
  )
  def test_multi_value_format_field(self, variant, expected_vcf_line):
    self.assertWrittenVCFRecordsEqual(
        self.write_variant_to_tempfile(variant), [expected_vcf_line])

  @parameterized.parameters(
      # Now let's combine some AD and DP fields.
      (_format_test_variant(['A', 'T', 'C'], [{
          'DP': 3,
          'AD': [0, 1, 2]
      }, {
          'DP': 12,
          'AD': [3, 4, 5]
      }]), _format_expected_variant('A', 'T,C', 'DP:AD', '3:0,1,2', '12:3,4,5')
      ),
      (_format_test_variant(['A', 'T', 'C'], [{
          'DP': 3
      }, {
          'AD': [3, 4, 5]
      }]), _format_expected_variant('A', 'T,C', 'DP:AD', '3:.', '.:3,4,5')),
  )
  def test_multiple_format_fields(self, variant, expected_vcf_line):
    self.assertWrittenVCFRecordsEqual(
        self.write_variant_to_tempfile(variant), [expected_vcf_line])


class VcfRoundtripTests(parameterized.TestCase):
  """Test the ability to round-trip VCF files."""

  def setUp(self):
    self.header = (
        '##fileformat=VCFv4.2\n'
        '##FILTER=<ID=PASS,Description="All filters passed">\n'
        '##INFO=<ID=DB,Number=0,Type=Flag,Description="In dbSNP">\n'
        '##INFO=<ID=MIN_DP,Number=1,Type=Integer,Description="Min DP">\n'
        '##FORMAT=<ID=GT,Number=1,Type=String,Description="Genotype">\n'
        '##FORMAT=<ID=AD,Number=R,Type=Integer,Description="Allelic depths">\n'
        '##FORMAT=<ID=DP,Number=1,Type=Integer,Description="Read depth">\n'
        '##contig=<ID=chr1,length=248956422>\n'
        '#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO\tFORMAT\tS1\tS2\n')
    self.record_format_strings = [
        'chr1\t13613\t.\tT\tA\t39.88\tPASS\t{info}\t{fmt}\t0/1{efmts1}\t1/1{efmts2}\n',
        'chr1\t13813\trs1\tT\tG\t90.28\tPASS\t{info}\t{fmt}\t1/1{efmts1}\t0|1{efmts2}\n',
        'chr1\t13838\t.\tC\tT\t62.74\tPASS\t{info}\t{fmt}\t0/1{efmts1}\t0/1{efmts2}\n',
    ]

  @parameterized.parameters(
      dict(
          expected_infos=['DB;MIN_DP=4', 'MIN_DP=15', 'DB;MIN_DP=10'],
          expected_fmt='GT:AD:DP',
          expected_fmt1=[':1,3:4', ':11,13:24', ':5,5:10'],
          expected_fmt2=[':1,19:20', ':7,8:15', ':.:10'],
      ),
      dict(
          expected_infos=['DB', '.', 'DB'],
          expected_fmt='GT:AD:DP',
          expected_fmt1=[':1,3:4', ':11,13:24', ':5,5:10'],
          expected_fmt2=[':1,19:20', ':7,8:15', ':.:10'],
          reader_excluded_info=['MIN_DP'],
      ),
      dict(
          expected_infos=['DB', '.', 'DB'],
          expected_fmt='GT',
          expected_fmt1=['', '', ''],
          expected_fmt2=['', '', ''],
          reader_excluded_info=['MIN_DP'],
          reader_excluded_format=['AD', 'DP'],
      ),
      dict(
          expected_infos=['DB', '.', 'DB'],
          expected_fmt='GT',
          expected_fmt1=['', '', ''],
          expected_fmt2=['', '', ''],
          writer_excluded_info=['MIN_DP'],
          writer_excluded_format=['AD', 'DP'],
      ),
      dict(
          expected_infos=['DB', '.', 'DB'],
          expected_fmt='GT',
          expected_fmt1=['', '', ''],
          expected_fmt2=['', '', ''],
          reader_excluded_info=['MIN_DP'],
          reader_excluded_format=['AD'],
          writer_excluded_info=['MIN_DP'],
          writer_excluded_format=['DP'],
      ),
  )
  def test_roundtrip(self,
                     expected_infos,
                     expected_fmt,
                     expected_fmt1,
                     expected_fmt2,
                     reader_excluded_info=None,
                     reader_excluded_format=None,
                     writer_excluded_info=None,
                     writer_excluded_format=None):
    expected_records = [
        record.format(info=info, fmt=expected_fmt, efmts1=e1,
                      efmts2=e2) for record, info, e1, e2 in zip(
                          self.record_format_strings, expected_infos,
                          expected_fmt1, expected_fmt2)
    ]
    expected = self.header + ''.join(expected_records)
    with vcf.VcfReader(
        test_utils.genomics_core_testdata('test_py_roundtrip.vcf'),
        use_index=False,
        excluded_info_fields=reader_excluded_info,
        excluded_format_fields=reader_excluded_format) as reader:

      records = list(reader.iterate())
      output_path = test_utils.test_tmpfile('test_roundtrip_tmpfile.vcf')
      with vcf.VcfWriter(
          output_path,
          header=reader.header,
          excluded_info_fields=writer_excluded_info,
          excluded_format_fields=writer_excluded_format) as writer:
        for record in records:
          writer.write(record)

    with open(output_path) as f:
      actual = f.read()
    self.assertEqual(actual, expected)


class InMemoryVcfReaderTests(parameterized.TestCase):
  """Test the functionality provided by vcf.InMemoryVcfReader."""

  def setUp(self):
    self.variants = [
        test_utils.make_variant(chrom='1', start=10),
        test_utils.make_variant(chrom='1', start=20),
        test_utils.make_variant(chrom='1', start=30),
        test_utils.make_variant(chrom='2', start=25),
        test_utils.make_variant(chrom='2', start=55),
        test_utils.make_variant(chrom='3', start=10),
    ]
    self.header = variants_pb2.VcfHeader(
        contigs=[
            reference_pb2.ContigInfo(name='1', n_bases=100),
            reference_pb2.ContigInfo(name='2', n_bases=100),
            reference_pb2.ContigInfo(name='3', n_bases=100),
            reference_pb2.ContigInfo(name='4', n_bases=100),
        ],
        filters=[],
        sample_names=['NA12878'])
    self.reader = vcf.InMemoryVcfReader(
        self.variants, self.header)

  def test_iterate(self):
    """Tests that iterate returns an iterable containing our variants."""
    self.assertEqual(list(self.reader.iterate()), self.variants)

  def test_header(self):
    """Tests that the reader provides us back the header we gave it."""
    self.assertEqual(self.reader.header, self.header)

  @parameterized.parameters(
      dict(query='1', expected_variant_indices=[0, 1, 2]),
      dict(query='2', expected_variant_indices=[3, 4]),
      dict(query='3', expected_variant_indices=[5]),
      dict(query='4', expected_variant_indices=[]),
      dict(query='1:1-15', expected_variant_indices=[0]),
      dict(query='1:1-25', expected_variant_indices=[0, 1]),
      dict(query='1:1-35', expected_variant_indices=[0, 1, 2]),
      dict(query='1:15-35', expected_variant_indices=[1, 2]),
      dict(query='1:25-35', expected_variant_indices=[2]),
  )
  def test_query(self, query, expected_variant_indices):
    range1 = ranges.parse_literal(query, ranges.contigs_dict(
        self.header.contigs))
    self.assertEqual(
        list(self.reader.query(range1)),
        [self.variants[i] for i in expected_variant_indices])


if __name__ == '__main__':
  absltest.main()
