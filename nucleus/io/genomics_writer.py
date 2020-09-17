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
"""Classes that provide the interface for writing genomics data.

`GenomicsWriter` defines the core API supported by writers, and is subclassed
directly or indirectly (via `DispatchingGenomicsWriter`) for all concrete
implementations.

`TFRecordWriter` is an implementation of the `GenomicsWriter` API for reading
`TFRecord` files. This is usable for all data types when writing data as
serialized protocol buffers.

`DispatchingGenomicsWriter` is an abstract class defined for convenience on top
of `GenomicsWriter` that supports writing to either the native file format or to
`TFRecord` files of the corresponding protocol buffer used to encode data of
that file type. The output format chosen is dependent upon the filename to which
the data are being written.

Concrete implementations for individual file types (e.g. BED, SAM, VCF, etc.)
reside in type-specific modules in this package. A general example of the write
functionality is shown below.

```python
# options is a writer-specific set of options.
options = ...

# records is an iterable of protocol buffers of the specific data type.
records = ...

with GenomicsWriterSubClass(output_path, options) as writer:
  for proto in records:
    writer.write(proto)
```
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import abc
import errno

from absl import logging

from nucleus.io.python import tfrecord_writer


class GenomicsWriter(object):
  """Abstract base class for writing genomics data.

  A GenomicsWriter only has one method, write, which writes a single
  protocol buffer to a file.
  """

  __metaclass__ = abc.ABCMeta

  @abc.abstractmethod
  def write(self, proto):
    """Writes proto to the file.

    Args:
      proto:  A protocol buffer.
    """

  def __enter__(self):
    """Enter a `with` block."""
    return self

  @abc.abstractmethod
  def __exit__(self, unused_type, unused_value, unused_traceback):
    """Exit a `with` block.  Typically, this will close the file."""


class TFRecordWriter(GenomicsWriter):
  """A GenomicsWriter that writes to a TFRecord file.

  Example usage:
    writer = TFRecordWriter('/tmp/my_output.tfrecord.gz')
    for record in records:
      writer.write(record)

  Note that TFRecord files do not need to be wrapped in a "with" block.
  """

  def __init__(self, output_path, header=None, compression_type=None):
    """Initializer.

    Args:
      output_path: str. The output path to which the records are written.
      header: An optional header for the particular data type. This can be
        useful for file types that have logical headers where some operations
        depend on that header information (e.g. VCF using its headers to
        determine type information of annotation fields).
      compression_type:  Either 'ZLIB', 'GZIP', '' (uncompressed), or
        None.  If None, __init__ will guess the compression type based on
        the input_path's suffix.

    Raises:
      IOError:  if there was any problem opening output_path for writing.
    """
    super(TFRecordWriter, self).__init__()
    self.header = header

    if compression_type is None:
      compression_type = 'GZIP' if output_path.endswith('.gz') else ''

    self._writer = tfrecord_writer.TFRecordWriter.from_file(
        output_path, compression_type)
    if self._writer is None:
      raise IOError(errno.EIO, 'Error opening %s for writing' % output_path)

  def write(self, proto):
    """Writes the proto to the TFRecord file."""
    self._writer.write(proto.SerializeToString())

  def __exit__(self, exit_type, exit_value, exit_traceback):
    self.close()

  def close(self):
    """Explicitly closes writer."""
    self._writer.close()


class DispatchingGenomicsWriter(GenomicsWriter):
  """A GenomicsWriter that dispatches based on the file extension.

  If '.tfrecord' is present in the filename, a TFRecordWriter is used.
  Otherwise, a native writer is.

  Sub-classes of DispatchingGenomicsWriter must define a _native_writer()
  method.
  """

  def __init__(self, output_path, **kwargs):
    """Initializer.

    Args:
      output_path: str. The output path to which the records are written.
      **kwargs: k=v named args. Keyword arguments used to instantiate the native
        writer, if applicable.
    """
    super(DispatchingGenomicsWriter, self).__init__()
    self.header = kwargs.get('header', None)

    if '.tfrecord' in output_path:
      self._writer = TFRecordWriter(output_path, header=self.header)
    else:
      self._writer = self._native_writer(output_path, **kwargs)
    logging.info('Writing %s with %s',
                 output_path, self._writer.__class__.__name__)
    self._post_init_hook()

  @abc.abstractmethod
  def _native_writer(self, output_path, **kwargs):
    """Returns a GenomicsWriter for writing the records `natively`.

    Args:
      output_path: The path to write the records to.
      **kwargs:  Zero or more keyword arguments.

    Returns:
      A GenomicsWriter.
    """

  def write(self, proto):
    self._writer.write(proto)

  def __exit__(self, exit_type, exit_value, exit_traceback):
    self._writer.__exit__(exit_type, exit_value, exit_traceback)

  def _post_init_hook(self):
    """Hook for subclasses to run code at the end of __init__."""
