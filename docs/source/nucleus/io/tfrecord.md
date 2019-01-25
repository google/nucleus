# nucleus.io.tfrecord -- I/O for TFRecord files.
**Source code:** [nucleus/io/tfrecord.py](https://github.com/google/nucleus/tree/master/nucleus/io/tfrecord.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
Utilities for reading and writing TFRecord files, especially those containing
serialized TensorFlow Example protocol buffers.

## Classes overview
Name | Description
-----|------------
[`RawProtoWriterAdaptor`](#rawprotowriteradaptor) | Adaptor class wrapping a low-level bytes writer with a write(proto) method.

## Functions overview
Name | Description
-----|------------
[`make_proto_writer`](#make_proto_writer)`(outfile)` | Returns a writer capable of writing general Protos to outfile.
[`make_tfrecord_options`](#make_tfrecord_options)`(filenames)` | Returns a python_io.TFRecordOptions for the specified filename.
[`make_tfrecord_writer`](#make_tfrecord_writer)`(outfile, options=None)` | Returns a python_io.TFRecordWriter for the specified outfile.
[`read_shard_sorted_tfrecords`](#read_shard_sorted_tfrecords)`(path, key, proto=None, max_records=None, options=None)` | Yields the parsed records in a TFRecord file path in sorted order.
[`read_tfrecords`](#read_tfrecords)`(path, proto=None, max_records=None, options=None)` | Yields the parsed records in a TFRecord file path.
[`write_tfrecords`](#write_tfrecords)`(protos, output_path, options=None)` | Writes protos to output_path.

## Classes
### RawProtoWriterAdaptor
```
Adaptor class wrapping a low-level bytes writer with a write(proto) method.

This class provides a simple wrapper around low-level writers that accept
serialized protobufs (via the SerializeToString()) for their write() methods.
After wrapping this low-level writer will have a write(proto) method that
accepts a protocol message `proto` and calls the low-level writer with
`proto.SerializeToString()`. Given that many C++ writers require the proto
to write properly (e.g., VCF writer), this allows us to provide a uniform API
to clients who call write(proto) and either have that write call go directly
to a type-specific writer or to a low-level writer via this
RawProtoWriterAdaptor.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, raw_writer, take_ownership=True)`
```
Creates a new RawProtoWriterAdaptor.

Arguments:
  raw_writer: A low-level writer with a write() method that accepts a
    serialized protobuf. Must also support __enter__ and __exit__ if
    take_ownership is True.
  take_ownership: bool. If True, we will call __enter__ and __exit__ on the
    raw_writer if/when this object's __enter__ and __exit__ are called. If
    False, no calls to these methods will be invoked on raw_writer.
```

<a name="write"></a>
##### `write(self, proto)`
```
Writes `proto.SerializeToString` to raw_writer.
```

## Functions
<a name="make_proto_writer"></a>
### `make_proto_writer(outfile)`
```
Returns a writer capable of writing general Protos to outfile.

Args:
  outfile: str. A path to a file where we want to write protos.

Returns:
  A writer object and a write_fn accepting a proto that writes to writer.
```

<a name="make_tfrecord_options"></a>
### `make_tfrecord_options(filenames)`
```
Returns a python_io.TFRecordOptions for the specified filename.

Args:
  filenames: str or list[str]. A path or a list of paths where we'll
    read/write our TFRecord.

Returns:
  A python_io.TFRecordOptions object.

Raises:
  ValueError: If the filenames contain inconsistent file types.
```

<a name="make_tfrecord_writer"></a>
### `make_tfrecord_writer(outfile, options=None)`
```
Returns a python_io.TFRecordWriter for the specified outfile.

Args:
  outfile: str. A path where we'll write our TFRecords.
  options: python_io.TFRecordOptions or None. If None, one
    will be inferred from the filename.

Returns:
  A python_io.TFRecordWriter object.
```

<a name="read_shard_sorted_tfrecords"></a>
### `read_shard_sorted_tfrecords(path, key, proto=None, max_records=None, options=None)`
```
Yields the parsed records in a TFRecord file path in sorted order.

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
  options: A python_io.TFRecordOptions object for the reader.

Yields:
  proto.FromString() values on each record in path in sorted order.
```

<a name="read_tfrecords"></a>
### `read_tfrecords(path, proto=None, max_records=None, options=None)`
```
Yields the parsed records in a TFRecord file path.

Note that path can be sharded filespec (path@N) in which case this function
will read each shard in order; i.e. shard 0 will read each entry in order,
then shard 1, ...

Args:
  path: String. A path to a TFRecord file containing protos.
  proto: A proto class. proto.FromString() will be called on each serialized
    record in path to parse it.
  max_records: int >= 0 or None. Maximum number of records to read from path.
    If None, the default, all records will be read.
  options: A python_io.TFRecordOptions object for the reader.

Yields:
  proto.FromString() values on each record in path in order.
```

<a name="write_tfrecords"></a>
### `write_tfrecords(protos, output_path, options=None)`
```
Writes protos to output_path.

This function writes serialized strings of each proto in protos to output_path
in their original order. If output_path is a sharded file (e.g., foo@2), this
function will write the protos spread out as evenly as possible among the
individual components of the sharded spec (e.g., foo-00000-of-00002 and
foo-00001-of-00002). Note that the order of records in the sharded files may
differ from the order in protos due to the striping.

Args:
  protos: An iterable of protobufs. The objects we want to write out.
  output_path: str. The filepath where we want to write protos.
  options: A python_io.TFRecordOptions object for the writer.
```

