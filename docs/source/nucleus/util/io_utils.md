# nucleus.util.io_utils -- Generic utilities for I/O.
**Source code:** [nucleus/util/io_utils.py](https://github.com/google/nucleus/tree/master/nucleus/util/io_utils.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
These utilities primarily support easy creation of `TFRecord` files and handle
reading and writing sharded files transparently.

## Classes overview
Name | Description
-----|------------
[`RawProtoWriterAdaptor`](#rawprotowriteradaptor) | Adaptor class wrapping a low-level bytes writer with a write(proto) method.
[`ShardError`](#sharderror) | An I/O error.

## Functions overview
Name | Description
-----|------------
[`GenerateShardedFilePattern`](#generateshardedfilepattern)`(basename, num_shards, suffix)` | Generate a sharded file pattern.
[`GenerateShardedFilenames`](#generateshardedfilenames)`(spec)` | Generate the list of filenames corresponding to the sharding path.
[`IsShardedFileSpec`](#isshardedfilespec)`(spec)` | Returns True if spec is a sharded file specification.
[`NormalizeToShardedFilePattern`](#normalizetoshardedfilepattern)`(spec_or_pattern)` | Take a sharding spec or sharding file pattern and return a sharded pattern.
[`ParseShardedFileSpec`](#parseshardedfilespec)`(spec)` | Parse a sharded file specification.
[`make_proto_writer`](#make_proto_writer)`(outfile)` | Returns a writer capable of writing general Protos to outfile.
[`make_tfrecord_options`](#make_tfrecord_options)`(filenames)` | Returns a python_io.TFRecordOptions for the specified filename.
[`make_tfrecord_writer`](#make_tfrecord_writer)`(outfile, options=None)` | Returns a python_io.TFRecordWriter for the specified outfile.
[`maybe_generate_sharded_filenames`](#maybe_generate_sharded_filenames)`(filespec)` | Potentially expands sharded filespec into a list of paths.
[`read_shard_sorted_tfrecords`](#read_shard_sorted_tfrecords)`(path, key, proto=None, max_records=None, options=None)` | Yields the parsed records in a TFRecord file path in sorted order.
[`read_tfrecords`](#read_tfrecords)`(path, proto=None, max_records=None, options=None)` | Yields the parsed records in a TFRecord file path.
[`resolve_filespecs`](#resolve_filespecs)`(shard, *filespecs)` | Transforms potentially sharded filespecs into their paths for single shard.
[`sharded_filename`](#sharded_filename)`(spec, i)` | Gets a path appropriate for writing the ith file of a sharded spec.
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

### ShardError
```
An I/O error.
```

## Functions
<a name="GenerateShardedFilePattern"></a>
### `GenerateShardedFilePattern(basename, num_shards, suffix)`
```
Generate a sharded file pattern.

Args:
  basename: str. The basename for the files.
  num_shards: int. The number of shards.
  suffix: str. The suffix if there is one or ''.
Returns:
  pattern:
```

<a name="GenerateShardedFilenames"></a>
### `GenerateShardedFilenames(spec)`
```
Generate the list of filenames corresponding to the sharding path.

Args:
  spec: str. Represents a filename with a sharding specification.
    e.g., 'gs://some/file@200.txt' represents a file sharded 200 ways.

Returns:
  List of filenames.

Raises:
  ShardError: If spec is not a valid sharded file specification.
```

<a name="IsShardedFileSpec"></a>
### `IsShardedFileSpec(spec)`
```
Returns True if spec is a sharded file specification.
```

<a name="NormalizeToShardedFilePattern"></a>
### `NormalizeToShardedFilePattern(spec_or_pattern)`
```
Take a sharding spec or sharding file pattern and return a sharded pattern.

The input can be a sharding spec(e.g '/some/file@10') or a sharded file
pattern (e.g. '/some/file-?????-of-00010)

Args:
  spec_or_pattern: str. A sharded file specification or sharded file pattern.

Returns:
  A sharded file pattern.
```

<a name="ParseShardedFileSpec"></a>
### `ParseShardedFileSpec(spec)`
```
Parse a sharded file specification.

Args:
  spec: str. The sharded file specification. A sharded file spec is one like
    'gs://some/file@200.txt'. Here, '@200' specifies the number of shards.

Returns:
  basename: The basename for the files.
  num_shards: The number of shards.
  suffix: The suffix if there is one, or '' if not.
Raises:
  ShardError: If the spec is not a valid sharded specification.
```

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

<a name="maybe_generate_sharded_filenames"></a>
### `maybe_generate_sharded_filenames(filespec)`
```
Potentially expands sharded filespec into a list of paths.

This function takes in a potentially sharded filespec and expands it into a
list containing the full set of corresponding concrete sharded file paths. If
the input filespec is not sharded then a list containing just that file path
is returned. This function is useful, for example, when the input to a binary
can either be sharded or not.

Args:
  filespec: String. A potentially sharded filespec to expand.

Returns:
  A list of file paths.

Raises:
  TypeError: if filespec is not in valid string_types.
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

<a name="resolve_filespecs"></a>
### `resolve_filespecs(shard, *filespecs)`
```
Transforms potentially sharded filespecs into their paths for single shard.

This function takes a shard number and a varargs of potentially-sharded
filespecs, and returns a list where the filespecs have been resolved into
concrete file paths for a single shard.

This function has a concept of a master filespec, which is used to constrain
and check the validity of other filespecs. The first filespec is considered
the master, and it cannot be None. For example, if master is not sharded, none
of the other specs can be sharded, and vice versa. They must all also have a
consistent sharding (e.g., master is @10, then all others must be @10).

Note that filespecs (except the master) may be None or any other False value,
which are returned as-is in the output list.

Args:
  shard: int >= 0. Our shard number.
  *filespecs: list[str]. Contains all of the filespecs we want to resolve into
    shard-specific file paths.

Returns:
  A list. The first element is the number of shards, followed by the
  shard-specific paths for each filespec, in order.

Raises:
  ValueError: if any filespecs are inconsistent.
```

<a name="sharded_filename"></a>
### `sharded_filename(spec, i)`
```
Gets a path appropriate for writing the ith file of a sharded spec.
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

