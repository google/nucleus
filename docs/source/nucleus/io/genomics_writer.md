# nucleus.io.genomics_writer -- Classes that provide the interface for writing genomics data.
**Source code:** [nucleus/io/genomics_writer.py](https://github.com/google/nucleus/tree/master/nucleus/io/genomics_writer.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
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

## Classes overview
Name | Description
-----|------------
[`DispatchingGenomicsWriter`](#dispatchinggenomicswriter) | A GenomicsWriter that dispatches based on the file extension.
[`GenomicsWriter`](#genomicswriter) | Abstract base class for writing genomics data.
[`TFRecordWriter`](#tfrecordwriter) | A GenomicsWriter that writes to a TFRecord file.

## Classes
### DispatchingGenomicsWriter
```
A GenomicsWriter that dispatches based on the file extension.

If '.tfrecord' is present in the filename, a TFRecordWriter is used.
Otherwise, a native writer is.

Sub-classes of DispatchingGenomicsWriter must define a _native_writer()
method.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, output_path, **kwargs)`
```
Initializer.

Args:
  output_path: str. The output path to which the records are written.
  **kwargs: k=v named args. Keyword arguments used to instantiate the native
    writer, if applicable.
```

<a name="write"></a>
##### `write(self, proto)`


### GenomicsWriter
```
Abstract base class for writing genomics data.

A GenomicsWriter only has one method, write, which writes a single
protocol buffer to a file.
```

#### Methods:
<a name="write"></a>
##### `write(self, proto)`
```
Writes proto to the file.

Args:
  proto:  A protocol buffer.
```

### TFRecordWriter
```
A GenomicsWriter that writes to a TFRecord file.

Example usage:
  writer = TFRecordWriter('/tmp/my_output.tfrecord.gz')
  for record in records:
    writer.write(record)

Note that TFRecord files do not need to be wrapped in a "with" block.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, output_path, header=None, compression_type=None)`
```
Initializer.

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
```

<a name="write"></a>
##### `write(self, proto)`
```
Writes the proto to the TFRecord file.
```

