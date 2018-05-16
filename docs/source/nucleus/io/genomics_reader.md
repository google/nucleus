# nucleus.io.genomics_reader -- Classes that provide the interface for reading genomics data.
**Source code:** [nucleus/io/genomics_reader.py](https://github.com/google/nucleus/tree/master/nucleus/io/genomics_reader.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
`GenomicsReader` defines the core API supported by readers, and is subclassed
directly or indirectly (via `DispatchingGenomicsReader`) for all concrete
implementations.

`TFRecordReader` is an implementation of the `GenomicsReader` API for reading
`TFRecord` files. This is usable for all data types when encoding data in
protocol buffers.

`DispatchingGenomicsReader` is an abstract class defined for convenience on top
of `GenomicsReader` that supports reading from either the native file format or
from `TFRecord` files of the corresponding protocol buffer used to encode data
of that file type. The input format assumed is dependent upon the filename of
the input data.

Concrete implementations for individual file types (e.g. BED, SAM, VCF, etc.)
reside in type-specific modules in this package. The instantiation of readers
may have reader-specific requirements documented there. General examples of the
`iterate()` and `query()` functionality are shown below.

```python
# Equivalent ways to iterate through all elements in a reader.
# 1. Using the reader itself as an iterable object.
kwargs = ...  # Reader-specific keyword arguments.
with GenomicsReaderSubClass(output_path, **kwargs) as reader:
  for proto in reader:
    do_something(reader.header, proto)

# 2. Calling the iterate() method of the reader explicitly.
with GenomicsReaderSubClass(output_path, **kwargs) as reader:
  for proto in reader.iterate():
    do_something(reader.header, proto)

# Querying for all elements within a specific region of the genome.
from nucleus.protos import range_pb2
region = range_pb2.Range(reference_name='chr1', start=10, end=20)

with GenomicsReaderSubClass(output_path, **kwargs) as reader:
  for proto in reader.query(region):
    do_something(reader.header, proto)
```

## Classes overview
Name | Description
-----|------------
[`DispatchingGenomicsReader`](#dispatchinggenomicsreader) | A GenomicsReader that dispatches based on the file extension.
[`GenomicsReader`](#genomicsreader) | Abstract base class for reading genomics data.
[`TFRecordReader`](#tfrecordreader) | A GenomicsReader that reads protocol buffers from a TFRecord file.

## Classes
### DispatchingGenomicsReader
```
A GenomicsReader that dispatches based on the file extension.

If '.tfrecord' is present in the filename, a TFRecordReader is used.
Otherwise, a native reader is.

Subclasses of DispatchingGenomicsReader must define the following methods:
  * _native_reader()
  * _record_proto()
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path, **kwargs)`


<a name="iterate"></a>
##### `iterate(self)`


<a name="query"></a>
##### `query(self, region)`


### GenomicsReader
```
Abstract base class for reading genomics data.

In addition to the abstractmethods defined below, subclasses should
also set a `header` member variable in their objects.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self)`
```
Initializer.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterator for going through all the file's records.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns an iterator for going through the records in the region.

Args:
  region:  A nucleus.genomics.v1.Range.

Returns:
  An iterator containing all and only records within the specified region.
```

### TFRecordReader
```
A GenomicsReader that reads protocol buffers from a TFRecord file.

Example usage:
  reader = TFRecordReader('/tmp/my_file.tfrecords.gz',
                          proto=tensorflow.Example)
  for example in reader:
    process(example)

Note that TFRecord files do not have headers, and do not need
to be wrapped in a "with" block.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path, proto, tf_options=None)`
```
Initializes the TFRecordReader.

Args:
  input_path:  The filename of the file to read.
  proto:  The protocol buffer type the TFRecord file is expected to
    contain.  For example, variants_pb2.Variant or reads_pb2.Read.
  tf_options:  A python_io.TFRecordOptions object.  If not set,
    __init__ will create one with the compression type based on
    whether input_path ends in '.gz' or not.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterator for going through all the file's records.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns an iterator for going through the records in the region.

NOTE: This function is not currently implemented by TFRecordReader as the
TFRecord format does not provide a general mechanism for fast random access
to elements in genome order.
```

