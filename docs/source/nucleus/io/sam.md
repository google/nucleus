# nucleus.io.sam -- Classes for reading and writing SAM and BAM files.
**Source code:** [nucleus/io/sam.py](https://github.com/google/nucleus/tree/master/nucleus/io/sam.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
The SAM/BAM format is described at
https://samtools.github.io/hts-specs/SAMv1.pdf

API for reading:

```python
from nucleus.io import sam

with sam.SamReader(input_path) as reader:
  for read in reader:
    print(read)
```

where `read` is a `nucleus.genomics.v1.Read` protocol buffer.

API for writing:

```python
from nucleus.io import sam

# reads is an iterable of nucleus.genomics.v1.Read protocol buffers.
reads = ...

with sam.SamWriter(output_path) as writer:
  for read in reads:
    writer.write(read)
```

For both reading and writing, if the path provided to the constructor contains
'.tfrecord' as an extension, a `TFRecord` file is assumed and attempted to be
read or written. Otherwise, the filename is treated as a true SAM/BAM file.

For `TFRecord` files, ending in a '.gz' suffix causes the file to be treated as
compressed with gzip.

## Classes overview
Name | Description
-----|------------
[`InMemorySamReader`](#inmemorysamreader) | Python interface class for in-memory SAM/BAM reader.
[`NativeSamReader`](#nativesamreader) | Class for reading from native SAM/BAM files.
[`NativeSamWriter`](#nativesamwriter) | Class for writing to native SAM/BAM files.
[`SamReader`](#samreader) | Class for reading Read protos from SAM or TFRecord files.
[`SamWriter`](#samwriter) | Class for writing Variant protos to SAM or TFRecord files.

## Classes
### InMemorySamReader
```
Python interface class for in-memory SAM/BAM reader.

Attributes:
  reads: list[nucleus.genomics.v1.Read]. The list of in-memory reads.
  is_sorted: bool, True if reads are sorted.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, reads, is_sorted=False)`


<a name="iterate"></a>
##### `iterate(self)`
```
Iterate over all records in the reads.

Returns:
  An iterator over nucleus.genomics.v1.Read's.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns an iterator for going through the reads in the region.

Args:
  region: nucleus.genomics.v1.Range. The query region.

Returns:
  An iterator over nucleus.genomics.v1.Read protos.
```

<a name="replace_reads"></a>
##### `replace_reads(self, reads, is_sorted=False)`
```
Replace the reads stored by this reader.
```

### NativeSamReader
```
Class for reading from native SAM/BAM files.

Most users will want to use SamReader instead, because it dynamically
dispatches between reading native SAM/BAM files and TFRecord files based
on the filename's extensions.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, input_path, read_requirements=None, parse_aux_fields=False, hts_block_size=None, downsample_fraction=None, random_seed=None)`
```
Initializes a NativeSamReader.

Args:
  input_path: str. A path to a resource containing SAM/BAM records.
    Currently supports SAM text format and BAM binary format.
  read_requirements: optional ReadRequirement proto. If not None, this proto
    is used to control which reads are filtered out by the reader before
    they are passed to the client.
  parse_aux_fields: optional bool, defaulting to False. If False, we do not
    parse the auxiliary fields of the SAM/BAM records (see SAM spec for
    details). Parsing the aux fields is unnecessary for many applications,
    and adds a significant parsing cost to access. If you need these aux
    fields, set parse_aux_fields to True and these fields will be parsed and
    populate the appropriate Read proto fields (e.g., read.info).
  hts_block_size: int or None. If specified, this configures the block size
    of the underlying htslib file object. Larger values (e.g. 1M) may be
    beneficial for reading remote files. If None, the reader uses the
    default htslib block size.
  downsample_fraction: float in the interval [0.0, 1.0] or None. If
    specified as a positive float, the reader will only keep each read with
    probability downsample_fraction, randomly. If None or zero, all reads
    are kept.
  random_seed: None or int. The random seed to use with this sam reader, if
    needed. If None, a fixed random value will be assigned.

Raises:
  ValueError: If downsample_fraction is not None and not in the interval
    (0.0, 1.0].
  ImportError: If someone tries to load a tfbam file.
```

<a name="iterate"></a>
##### `iterate(self)`
```
Returns an iterable of Read protos in the file.
```

<a name="query"></a>
##### `query(self, region)`
```
Returns an iterator for going through the reads in the region.
```

### NativeSamWriter
```
Class for writing to native SAM/BAM files.

Most users will want SamWriter, which will write to either native SAM/BAM
files or TFRecords files, based on the output filename's extensions.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, output_path, header)`
```
Initializer for NativeSamWriter.

Args:
  output_path: str. A path where we'll write our SAM/BAM file.
  header: A nucleus.SamHeader proto.  The header is used both for writing
    the header, and to control the sorting applied to the rest of the file.
```

<a name="write"></a>
##### `write(self, proto)`


### SamReader
```
Class for reading Read protos from SAM or TFRecord files.
```

### SamWriter
```
Class for writing Variant protos to SAM or TFRecord files.
```

