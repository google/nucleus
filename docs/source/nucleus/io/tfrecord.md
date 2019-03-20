# nucleus.io.tfrecord -- I/O for TFRecord files.
**Source code:** [nucleus/io/tfrecord.py](https://github.com/google/nucleus/tree/master/nucleus/io/tfrecord.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
Utilities for reading and writing TFRecord files, especially those containing
serialized TensorFlow Example protocol buffers.

## Functions overview
Name | Description
-----|------------
[`Reader`](#reader)`(path, proto=None, compression_type=None)` | A TFRecordReader that defaults to tf.Example protos.
[`Writer`](#writer)`(path, compression_type=None)` | A convenience wrapper around genomics_writer.TFRecordWriter.
[`read_shard_sorted_tfrecords`](#read_shard_sorted_tfrecords)`(path, key, proto=None, max_records=None, compression_type=None)` | Yields the parsed records in a TFRecord file path in sorted order.
[`read_tfrecords`](#read_tfrecords)`(path, proto=None, max_records=None, compression_type=None)` | Yields the parsed records in a TFRecord file path.
[`write_tfrecords`](#write_tfrecords)`(protos, output_path, compression_type=None)` | Writes protos to output_path.

## Functions
<a name="Reader"></a>
### `Reader(path, proto=None, compression_type=None)`
```
A TFRecordReader that defaults to tf.Example protos.
```

<a name="Writer"></a>
### `Writer(path, compression_type=None)`
```
A convenience wrapper around genomics_writer.TFRecordWriter.
```

<a name="read_shard_sorted_tfrecords"></a>
### `read_shard_sorted_tfrecords(path, key, proto=None, max_records=None, compression_type=None)`
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
  compression_type: 'GZIP', 'ZLIB', '' (uncompressed), or None to autodetect
    based on file extension.

Yields:
  proto.FromString() values on each record in path in sorted order.
```

<a name="read_tfrecords"></a>
### `read_tfrecords(path, proto=None, max_records=None, compression_type=None)`
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
  compression_type: 'GZIP', 'ZLIB', '' (uncompressed), or None to autodetect
    based on file extension.

Yields:
  proto.FromString() values on each record in path in order.
```

<a name="write_tfrecords"></a>
### `write_tfrecords(protos, output_path, compression_type=None)`
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
  compression_type: 'GZIP', 'ZLIB', '' (uncompressed), or None to autodetect
    based on file extension.
```

