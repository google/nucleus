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
r"""Tests for nucleus.examples.add_ad_to_vcf.

Generating the golden output:

./nucleus/examples/ngs_errors \
  --ref nucleus/testdata/ucsc.hg19.chr20.unittest.fasta.gz \
  --vcf nucleus/testdata/test_nist.b37_chr20_100kbp_at_10mb.vcf.gz \
  --bam nucleus/testdata/NA12878_S1.chr20.10_10p1mb.bam \
  --examples_out nucleus/testdata/golden.examples.ngs_errors.tfrecord \
  --max_reads 100
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest

from nucleus.examples import ngs_errors
from nucleus.io import genomics_reader
from nucleus.testing import test_utils
from tensorflow.core.example import example_pb2


def _read_examples(path):
  with genomics_reader.TFRecordReader(path, proto=example_pb2.Example) as fin:
    return list(fin)


# pylint: disable=invalid-name
def assertExamplesAreEqual(self, expected, actual, expected_keys=None):
  for expected1, actual1 in zip(expected, actual):
    self.assertCountEqual(expected1.features.feature.keys(),
                          actual1.features.feature.keys())

    if expected_keys is not None:
      self.assertCountEqual(expected_keys, actual1.features.feature.keys())

    for key in expected1.features.feature.keys():
      self.assertEqual(expected1.features.feature[key],
                       actual1.features.feature[key])
# pylint: enable=invalid-name


class NGSErrorsTest(absltest.TestCase):

  def test_main(self):
    examples_out = test_utils.test_tmpfile('output.tfrecord')
    ngs_errors.make_ngs_error_examples(
        ref_path=test_utils.genomics_core_testdata(
            'ucsc.hg19.chr20.unittest.fasta.gz'),
        vcf_path=test_utils.genomics_core_testdata(
            'test_nist.b37_chr20_100kbp_at_10mb.vcf.gz'),
        bam_path=test_utils.genomics_core_testdata(
            'NA12878_S1.chr20.10_10p1mb.bam'),
        examples_out_path=examples_out,
        max_reads=100)

    actual_examples = _read_examples(examples_out)
    golden_examples = _read_examples(
        test_utils.genomics_core_testdata(
            'golden.examples.ngs_errors.tfrecord'))

    self.assertEqual(len(actual_examples), 100)
    assertExamplesAreEqual(
        self,
        golden_examples,
        actual_examples,
        expected_keys={
            'read_name', 'cigar', 'read_sequence', 'read_qualities',
            'true_sequence'
        })


if __name__ == '__main__':
  absltest.main()
