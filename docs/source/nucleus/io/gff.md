# nucleus.io.gff -- Classes for reading and writing GFF files.
**Source code:** [nucleus/io/gff.py](https://github.com/google/nucleus/tree/master/nucleus/io/gff.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
The GFF format is described at
https://github.com/The-Sequence-Ontology/Specifications/blob/master/gff3.md.

API for reading:

```python
from nucleus.io import gff

# Iterate through all records.
with gff.GffReader(input_path) as reader:
  for record in reader:
    print(record)
```

where `record` is a `nucleus.genomics.v1.GffRecord` protocol buffer.

API for writing:

```python
from nucleus.io import gff
from nucleus.protos import gff_pb2

# records is an iterable of nucleus.genomics.v1.GffRecord protocol buffers.
records = ...

header = gff_pb2.GffHeader()

# Write all records to the desired output path.
with gff.GffWriter(output_path, header) as writer:
  for record in records:
    writer.write(record)
```

For both reading and writing, if the path provided to the constructor contains
'.tfrecord' as an extension, a `TFRecord` file is assumed and attempted to be
read or written. Otherwise, the filename is treated as a true GFF file.

Files that end in a '.gz' suffix cause the file to be treated as compressed
(with BGZF if it is a true GFF file, and with gzip if it is a TFRecord file).

## Classes overview
Name | Description
-----|------------
[`GffReader`](#gffreader) | Class for reading GffRecord protos from GFF or TFRecord files.
[`GffWriter`](#gffwriter) | Class for writing GffRecord protos to GFF or TFRecord files.
[`NativeGffReader`](#nativegffreader) | Class for reading from native GFF files.
[`NativeGffWriter`](#nativegffwriter) | Class for writing to native GFF files.

## Classes
### GffReader
```
Class for reading GffRecord protos from GFF or TFRecord files.
```

### GffWriter
```
Class for writing GffRecord protos to GFF or TFRecord files.
```

### NativeGffReader
```
Class for reading from native GFF files.

Most users will want to use GffReader instead, because it dynamically
dispatches between reading native GFF files and TFRecord files based on the
filename's extension.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path)`
```
Initializes a NativeGffReader.

Args:
  input_path: string. A path to a resource containing GFF records.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of GffRecord protos in the file.
```

<a name="query"></a>
##### `query(self)`
```
Returns an iterator for going through the records in the region.

NOTE: This function is not currently implemented by NativeGffReader though
it could be implemented for sorted, tabix-indexed GFF files.
```

### NativeGffWriter
```
Class for writing to native GFF files.

Most users will want GffWriter, which will write to either native GFF
files or TFRecord files, based on the output filename's extension.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, output_path, header)`
```
Initializer for NativeGffWriter.

Args:
  output_path: str. The path to which to write the GFF file.
  header: nucleus.genomics.v1.GffHeader. The header that defines all
    information germane to the constituent GFF records.
```

<a name="write"></a>
##### `write(self, proto)`


