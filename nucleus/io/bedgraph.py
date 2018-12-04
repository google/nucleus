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
"""Classes for reading and writing BedGraph files.

The BedGraph format is described at
https://genome.ucsc.edu/goldenpath/help/bedgraph.html

API for reading:

```python
from nucleus.io import bedgraph

# Iterate through all records.
with bed.BedGraphReader(input_path) as reader:
  for record in reader:
    print(record)
```

where `record` is a `nucleus.genomics.v1.BedGraphRecord` protocol buffer.

API for writing:

```python
from nucleus.io import bedgraph
from nucleus.protos import bedgraph_pb2

# records is an iterable of nucleus.genomics.v1.BedGraphRecord protocol buffers.
records = ...

# Write all records to the desired output path.
with bed.BedGraphWriter(output_path) as writer:
  for record in records:
    writer.write(record)
```

For both reading and writing, if the path provided to the constructor contains
'.tfrecord' as an extension, a `TFRecord` file is assumed and attempted to be
read or written. Otherwise, the filename is treated as a true BedGraph file.

Files that end in a '.gz' suffix cause the file to be treated as compressed
(with BGZF if it is a BedGraph file, and with gzip if it is a TFRecord file).
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from nucleus.io import genomics_reader
from nucleus.io import genomics_writer
from nucleus.io.python import bedgraph_reader
from nucleus.io.python import bedgraph_writer
from nucleus.protos import bedgraph_pb2


class NativeBedGraphReader(genomics_reader.GenomicsReader):
  """Class for reading from native BedGraph files.

  Most users will want to use BedGraphReader instead, because it dynamically
  dispatches between reading native BedGraph files and TFRecord files based on
  the filename's extension.
  """

  def __init__(self, input_path, num_fields=0):
    """Initializes a NativeBedGraphReader.

    Args:
      input_path: string. A path to a resource containing BedGraph records.
      num_fields: int. The number of fields to read in the BedGraph. If unset or
        set to zero, all fields in the input are read.
    """
    super(NativeBedGraphReader, self).__init__()

    bedgraph_path = input_path.encode('utf8')
    self._reader = bedgraph_reader.BedGraphReader.from_file(bedgraph_path)

  def query(self):
    """Returns an iterator for going through the records in the region.

    NOTE: This function is not currently implemented by NativeBedGraphReader
    though it could be implemented for sorted, tabix-indexed BedGraph files.
    """
    raise NotImplementedError('Can not currently query a BedGraph file')

  def iterate(self):
    """Returns an iterable of BedGraphRecord protos in the file."""
    return self._reader.iterate()

  def __exit__(self, exit_type, exit_value, exit_traceback):
    self._reader.__exit__(exit_type, exit_value, exit_traceback)


class BedGraphReader(genomics_reader.DispatchingGenomicsReader):
  """Class for reading BedGraphRecord protos from BedGraph or TFRecord files."""

  def _native_reader(self, input_path, **kwargs):
    return NativeBedGraphReader(input_path, **kwargs)

  def _record_proto(self):
    return bedgraph_pb2.BedGraphRecord


class NativeBedGraphWriter(genomics_writer.GenomicsWriter):
  """Class for writing to native BedGraph files.

  Most users will want BedGraphWriter, which will write to either native
  BedGraph files or TFRecord files, based on the output filename's extension.
  """

  def __init__(self, output_path, header=None):
    """Initializer for NativeBedGraphWriter.

    Args:
      output_path: str. The path to which to write the BedGraph file.
    """
    super(NativeBedGraphWriter, self).__init__()
    self._writer = bedgraph_writer.BedGraphWriter.to_file(output_path)

  def write(self, proto):
    self._writer.write(proto)

  def __exit__(self, exit_type, exit_value, exit_traceback):
    self._writer.__exit__(exit_type, exit_value, exit_traceback)


class BedGraphWriter(genomics_writer.DispatchingGenomicsWriter):
  """Class for writing BedGraphRecord protos to BedGraph or TFRecord files."""

  def _native_writer(self, output_path):
    return NativeBedGraphWriter(output_path)
