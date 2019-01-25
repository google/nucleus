# nucleus.io.bedgraph -- Classes for reading and writing BedGraph files.
**Source code:** [nucleus/io/bedgraph.py](https://github.com/google/nucleus/tree/master/nucleus/io/bedgraph.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
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

## Classes overview
Name | Description
-----|------------
[`BedGraphReader`](#bedgraphreader) | Class for reading BedGraphRecord protos from BedGraph or TFRecord files.
[`BedGraphWriter`](#bedgraphwriter) | Class for writing BedGraphRecord protos to BedGraph or TFRecord files.
[`NativeBedGraphReader`](#nativebedgraphreader) | Class for reading from native BedGraph files.
[`NativeBedGraphWriter`](#nativebedgraphwriter) | Class for writing to native BedGraph files.

## Classes
### BedGraphReader
```
Class for reading BedGraphRecord protos from BedGraph or TFRecord files.
```

### BedGraphWriter
```
Class for writing BedGraphRecord protos to BedGraph or TFRecord files.
```

### NativeBedGraphReader
```
Class for reading from native BedGraph files.

Most users will want to use BedGraphReader instead, because it dynamically
dispatches between reading native BedGraph files and TFRecord files based on
the filename's extension.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path, num_fields=0)`
```
Initializes a NativeBedGraphReader.

Args:
  input_path: string. A path to a resource containing BedGraph records.
  num_fields: int. The number of fields to read in the BedGraph. If unset or
    set to zero, all fields in the input are read.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of BedGraphRecord protos in the file.
```

<a name="query"></a>
##### `query(self)`
```
Returns an iterator for going through the records in the region.

NOTE: This function is not currently implemented by NativeBedGraphReader
though it could be implemented for sorted, tabix-indexed BedGraph files.
```

### NativeBedGraphWriter
```
Class for writing to native BedGraph files.

Most users will want BedGraphWriter, which will write to either native
BedGraph files or TFRecord files, based on the output filename's extension.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, output_path, header=None)`
```
Initializer for NativeBedGraphWriter.

Args:
  output_path: str. The path to which to write the BedGraph file.
```

<a name="write"></a>
##### `write(self, proto)`


