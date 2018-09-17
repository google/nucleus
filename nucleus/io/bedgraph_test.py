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
"""Tests for nucleus.io.bedgraph."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import bedgraph
from nucleus.protos import bedgraph_pb2
from nucleus.testing import test_utils


class BedGraphTests(parameterized.TestCase):

  @parameterized.parameters('test_regions.bedgraph', 'test_regions.bedgraph.gz')
  def test_iterate_bedgraph_reader(self, bedgraph_path):
    bedgraph_path = test_utils.genomics_core_testdata(bedgraph_path)
    expected = [('chr1', 10, 20, 100), ('chr1', 100, 200, 250),
                ('chr1', 300, 400, 150.1), ('chr1', 500, 501, 20.13)]
    with bedgraph.BedGraphReader(bedgraph_path) as reader:
      records = list(reader.iterate())
    self.assertLen(records, 4)
    self.assertEqual(
        [(r.reference_name, r.start, r.end, r.data_value) for r in records],
        expected)

  @parameterized.parameters('test_regions.bedgraph', 'test_regions.bedgraph.gz')
  def test_roundtrip_writer(self, bedgraph_path):
    output_path = test_utils.test_tmpfile(bedgraph_path)
    input_path = test_utils.genomics_core_testdata(bedgraph_path)
    records = []
    with bedgraph.BedGraphReader(input_path) as reader:
      records = list(reader.iterate())

    with bedgraph.BedGraphWriter(output_path) as writer:
      for record in records:
        writer.write(record)

    with bedgraph.BedGraphReader(output_path) as reader:
      v2_records = list(reader.iterate())

    self.assertLen(records, 4)
    self.assertEqual(records, v2_records)


if __name__ == '__main__':
  absltest.main()
