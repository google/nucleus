# nucleus.io.bed -- Classes for reading and writing BED files.
**Source code:** [nucleus/io/bed.py](https://github.com/google/nucleus/tree/master/nucleus/io/bed.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
The BED format is described at
https://genome.ucsc.edu/FAQ/FAQformat.html#format1

API for reading:

```python
from nucleus.io import bed

# Iterate through all records.
with bed.BedReader(input_path) as reader:
  for record in reader:
    print(record)
```

where `record` is a `nucleus.genomics.v1.BedRecord` protocol buffer.

API for writing:

```python
from nucleus.io import bed
from nucleus.protos import bed_pb2

# records is an iterable of nucleus.genomics.v1.BedRecord protocol buffers.
records = ...

# header defines how many fields to write out.
header = bed_pb2.BedHeader(num_fields=5)

# Write all records to the desired output path.
with bed.BedWriter(output_path, header) as writer:
  for record in records:
    writer.write(record)
```

For both reading and writing, if the path provided to the constructor contains
'.tfrecord' as an extension, a `TFRecord` file is assumed and attempted to be
read or written. Otherwise, the filename is treated as a true BED file.

Files that end in a '.gz' suffix cause the file to be treated as compressed
(with BGZF if it is a true BED file, and with gzip if it is a TFRecord file).

## Classes overview
Name | Description
-----|------------
[`BedReader`](#bedreader) | Class for reading BedRecord protos from BED or TFRecord files.
[`BedWriter`](#bedwriter) | Class for writing BedRecord protos to BED or TFRecord files.
[`NativeBedReader`](#nativebedreader) | Class for reading from native BED files.
[`NativeBedWriter`](#nativebedwriter) | Class for writing to native BED files.

## Classes
### BedReader
```
Class for reading BedRecord protos from BED or TFRecord files.
```

### BedWriter
```
Class for writing BedRecord protos to BED or TFRecord files.
```

### NativeBedReader
```
Class for reading from native BED files.

Most users will want to use BedReader instead, because it dynamically
dispatches between reading native BED files and TFRecord files based on the
filename's extension.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path, num_fields=0)`
```
Initializes a NativeBedReader.

Args:
  input_path: string. A path to a resource containing BED records.
  num_fields: int. The number of fields to read in the BED. If unset or set
    to zero, all fields in the input are read.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of BedRecord protos in the file.
```

<a name="query"></a>
##### `query(self)`
```
Returns an iterator for going through the records in the region.

NOTE: This function is not currently implemented by NativeBedReader though
it could be implemented for sorted, tabix-indexed BED files.
```

### NativeBedWriter
```
Class for writing to native BED files.

Most users will want BedWriter, which will write to either native BED
files or TFRecord files, based on the output filename's extension.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, output_path, header=None)`
```
Initializer for NativeBedWriter.

Args:
  output_path: str. The path to which to write the BED file.
  header: nucleus.genomics.v1.BedHeader. The header that defines all
    information germane to the constituent BED records.
```

<a name="write"></a>
##### `write(self, proto)`


