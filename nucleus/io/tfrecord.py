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
"""I/O for TFRecord files.

Utilities for reading and writing TFRecord files, especially those containing
serialized TensorFlow Example protocol buffers.
"""

# Important: Please keep this module free of TensorFlow C++ extensions.
# This makes it easy to build pure python packages for training that work with
# CMLE.

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import heapq

import contextlib2

from nucleus.io import genomics_reader
from nucleus.io import genomics_writer
from nucleus.io import sharded_file_utils
from nucleus.protos import example_pb2


# pylint: disable=invalid-name
def Reader(path, proto=None, compression_type=None):
  """A TFRecordReader that defaults to tf.Example protos."""
  if not proto:
    proto = example_pb2.Example

  return genomics_reader.TFRecordReader(
      path, proto, compression_type=compression_type)


def Writer(path, compression_type=None):
  """A convenience wrapper around genomics_writer.TFRecordWriter."""
  return genomics_writer.TFRecordWriter(
      path, compression_type=compression_type)
# pylint: enable=invalid-name


# TODO(thomaswc): Refactor all of the following (b/128406743).
def read_tfrecords(path, proto=None, max_records=None, compression_type=None):
  """Yields the parsed records in a TFRecord file path.

  Note that path can be sharded filespec (path@N) in which case this function
  will read each shard in order; i.e. shard 0 will read each entry in order,
  then shard 1, ...

  Args:
    path: String. A path to a TFRecord file containing protos.
    proto: A proto class. proto.FromString() will be called on each serialized
      record in path to parse it.
    max_records: int >= 0 or None. Maximum number of records to read from path.
      If None, the default, all records will be read.
    compression_type: 'GZIP', 'ZLIB', '' (uncompressed), or None to autodetect
      based on file extension.

  Yields:
    proto.FromString() values on each record in path in order.
  """
  if sharded_file_utils.is_sharded_file_spec(path):
    paths = sharded_file_utils.generate_sharded_filenames(path)
  else:
    paths = [path]

  i = 0
  for path in paths:
    with Reader(path, proto, compression_type=compression_type) as reader:
      for record in reader.iterate():
        i += 1
        if max_records is not None and i > max_records:
          return
        yield record


def read_shard_sorted_tfrecords(path,
                                key,
                                proto=None,
                                max_records=None,
                                compression_type=None):
  """Yields the parsed records in a TFRecord file path in sorted order.

  The input TFRecord file must have each shard already in sorted order when
  using the key function for comparison (but elements can be interleaved across
  shards). Under those constraints, the elements will be yielded in a global
  sorted order.

  Args:
    path: String. A path to a TFRecord-formatted file containing protos.
    key: Callable. A function that takes as input a single instance of the proto
      class and returns a value on which the comparison for sorted ordering is
      performed.
    proto: A proto class. proto.FromString() will be called on each serialized
      record in path to parse it.
    max_records: int >= 0 or None. Maximum number of records to read from path.
      If None, the default, all records will be read.
    compression_type: 'GZIP', 'ZLIB', '' (uncompressed), or None to autodetect
      based on file extension.

  Yields:
    proto.FromString() values on each record in path in sorted order.
  """
  if sharded_file_utils.is_sharded_file_spec(path):
    paths = sharded_file_utils.generate_sharded_filenames(path)
  else:
    paths = [path]

  keyed_iterables = []
  for path in paths:
    protos = Reader(path, proto, compression_type=compression_type).iterate()
    keyed_iterables.append(((key(elem), elem) for elem in protos))

  for i, (_, value) in enumerate(heapq.merge(*keyed_iterables)):
    if max_records is not None and i >= max_records:
      return
    yield value


def write_tfrecords(protos, output_path, compression_type=None):
  """Writes protos to output_path.

  This function writes serialized strings of each proto in protos to output_path
  in their original order. If output_path is a sharded file (e.g., foo@2), this
  function will write the protos spread out as evenly as possible among the
  individual components of the sharded spec (e.g., foo-00000-of-00002 and
  foo-00001-of-00002). Note that the order of records in the sharded files may
  differ from the order in protos due to the striping.

  Args:
    protos: An iterable of protobufs. The objects we want to write out.
    output_path: str. The filepath where we want to write protos.
    compression_type: 'GZIP', 'ZLIB', '' (uncompressed), or None to autodetect
      based on file extension.
  """
  if sharded_file_utils.is_sharded_file_spec(output_path):
    with contextlib2.ExitStack() as stack:
      _, n_shards, _ = sharded_file_utils.parse_sharded_file_spec(output_path)
      writers = [
          stack.enter_context(
              Writer(sharded_file_utils.sharded_filename(
                  output_path, i), compression_type=compression_type))
          for i in range(n_shards)
      ]
      for i, proto in enumerate(protos):
        writers[i % n_shards].write(proto)
  else:
    with Writer(output_path, compression_type=compression_type) as writer:
      for proto in protos:
        writer.write(proto)
