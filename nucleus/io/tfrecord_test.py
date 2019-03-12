# Copyright 2019 Google LLC.
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

"""Tests for nucleus.io.tfrecord."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import types

from absl.testing import absltest
from absl.testing import parameterized

from nucleus.io import tfrecord

from nucleus.protos import reference_pb2
from nucleus.testing import test_utils


class IOTest(parameterized.TestCase):

  def write_test_protos(self, filename):
    protos = [reference_pb2.ContigInfo(name=str(i)) for i in range(10)]
    path = test_utils.test_tmpfile(filename)
    tfrecord.write_tfrecords(protos, path)
    return protos, path

  @parameterized.parameters('foo.tfrecord', 'foo@2.tfrecord', 'foo@3.tfrecord')
  def test_read_write_tfrecords(self, filename):
    protos, path = self.write_test_protos(filename)

    # Create our generator of records from read_tfrecords.
    reader = tfrecord.read_tfrecords(path, reference_pb2.ContigInfo)

    # Make sure it's actually a generator.
    self.assertEqual(type(reader), types.GeneratorType)

    # Check the round-trip contents.
    if '@' in filename:
      # Sharded outputs are striped across shards, so order isn't preserved.
      self.assertCountEqual(protos, reader)
    else:
      self.assertEqual(protos, list(reader))

  @parameterized.parameters((filename, max_records)
                            for max_records in [None, 0, 1, 3, 100]
                            for filename in ['foo.tfrecord', 'foo@2.tfrecord'])
  def test_read_tfrecords_max_records(self, filename, max_records):
    protos, path = self.write_test_protos(filename)

    # Create our generator of records from read_tfrecords.
    if max_records is None:
      expected_n = len(protos)
    else:
      expected_n = min(max_records, len(protos))
    actual = tfrecord.read_tfrecords(
        path, reference_pb2.ContigInfo, max_records=max_records)
    self.assertLen(list(actual), expected_n)

  @parameterized.parameters('foo.tfrecord', 'foo@2.tfrecord', 'foo@3.tfrecord')
  def test_shard_sorted_tfrecords(self, filename):
    protos, path = self.write_test_protos(filename)

    # Create our generator of records.
    key = lambda x: int(x.name)
    reader = tfrecord.read_shard_sorted_tfrecords(
        path, key=key, proto=reference_pb2.ContigInfo)

    # Make sure it's actually a generator.
    self.assertEqual(type(reader), types.GeneratorType)

    # Check the round-trip contents.
    contents = list(reader)
    self.assertEqual(protos, contents)
    self.assertEqual(contents, sorted(contents, key=key))

  @parameterized.parameters((filename, max_records)
                            for max_records in [None, 0, 1, 3, 100]
                            for filename in ['foo.tfrecord', 'foo@2.tfrecord'])
  def test_shard_sorted_tfrecords_max_records(self, filename, max_records):
    protos, path = self.write_test_protos(filename)

    if max_records is None:
      expected_n = len(protos)
    else:
      expected_n = min(max_records, len(protos))
    # Create our generator of records from read_tfrecords.
    actual = tfrecord.read_shard_sorted_tfrecords(
        path,
        key=lambda x: int(x.name),
        proto=reference_pb2.ContigInfo,
        max_records=max_records)
    self.assertLen(list(actual), expected_n)


if __name__ == '__main__':
  absltest.main()
