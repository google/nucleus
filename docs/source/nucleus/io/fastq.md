# nucleus.io.fastq -- Classes for reading and writing FASTQ files.
**Source code:** [nucleus/io/fastq.py](https://github.com/google/nucleus/tree/master/nucleus/io/fastq.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
The FASTQ format is described at
https://en.wikipedia.org/wiki/FASTQ_format

API for reading:

```python
from nucleus.io import fastq

with fastq.FastqReader(input_path) as reader:
  for record in reader:
    print(record)
```

where `record` is a `nucleus.genomics.v1.FastqRecord` protocol buffer.

API for writing:

```python
from nucleus.io import fastq

# records is an iterable of nucleus.genomics.v1.FastqRecord protocol buffers.
records = ...

with fastq.FastqWriter(output_path) as writer:
  for record in records:
    writer.write(record)
```

For both reading and writing, if the path provided to the constructor contains
'.tfrecord' as an extension, a `TFRecord` file is assumed and attempted to be
read or written. Otherwise, the filename is treated as a true FASTQ file.

Files that end in a '.gz' suffix cause the file to be treated as compressed
(with BGZF if it is a true FASTQ file, and with gzip if it is a TFRecord file).

## Classes overview
Name | Description
-----|------------
[`FastqReader`](#fastqreader) | Class for reading FastqRecord protos from FASTQ or TFRecord files.
[`FastqWriter`](#fastqwriter) | Class for writing FastqRecord protos to FASTQ or TFRecord files.
[`NativeFastqReader`](#nativefastqreader) | Class for reading from native FASTQ files.
[`NativeFastqWriter`](#nativefastqwriter) | Class for writing to native FASTQ files.

## Classes
### FastqReader
```
Class for reading FastqRecord protos from FASTQ or TFRecord files.
```

### FastqWriter
```
Class for writing FastqRecord protos to FASTQ or TFRecord files.
```

### NativeFastqReader
```
Class for reading from native FASTQ files.

Most users will want to use FastqReader instead, because it dynamically
dispatches between reading native FASTQ files and TFRecord files based on the
filename's extension.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path)`
```
Initializes a NativeFastqReader.

Args:
  input_path: str. A path to a resource containing FASTQ records.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of FastqRecord protos in the file.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns an iterator for going through the records in the region.

NOTE: This function is not implemented by NativeFastqReader as there is no
concept of genome ordering in the FASTQ format.
```

### NativeFastqWriter
```
Class for writing to native FASTQ files.

Most users will want FastqWriter, which will write to either native FASTQ
files or TFRecord files, based on the output filename's extension.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, output_path)`
```
Initializer for NativeFastqWriter.

Args:
  output_path: str. The path to which to write the FASTQ file.
```

<a name="write"></a>
##### `write(self, proto)`


