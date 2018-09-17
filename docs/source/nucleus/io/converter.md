# nucleus.io.converter -- A universal converter program for nucleus-supported genomics file formats.
**Source code:** [nucleus/io/converter.py](https://github.com/google/nucleus/tree/master/nucleus/io/converter.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
Invoked with a single argument, this program will open a genomics data file and
iterate over its contents, doing no writing.  This is a good benchmark for I/O
and reader processing speed.

Invoked with two arguments, the program will open the first file, read its
records, and write them, one at a time, to the second file.  The filetypes for
the first and second filename must be compatible ways of encoding the same
nucleus genomics record type (for example, `infile.gff` and
`outfile.gff.tfrecord.gz` are compatible, but `infile.gff` and `outfile.bam` are
not.

Note: at present we have no convention for encoding a file *header* in
tfrecords, so conversion is not possible from tfrecord to any native file format
for which a header is compulsory.

## Classes overview
Name | Description
-----|------------
[`ConversionError`](#conversionerror) | An exception used to signal file conversion error.
[`NullWriter`](#nullwriter) | A writer class whose .write() method is a no-op.

## Functions overview
Name | Description
-----|------------
[`convert`](#convert)`(in_filename, out_filename)` | Converts a recognized genomics file `in_filename` to `out_filename`.
[`main`](#main)`(argv)` | 

## Classes
### ConversionError
```
An exception used to signal file conversion error.
```

### NullWriter
```
A writer class whose .write() method is a no-op.

This allows us to create and use a writer object where one is required by
context but we do not wish to write to any file.
```

#### Methods:
<a name="__init__"></a>
##### `__init__(self, unused_filename, header=None)`


<a name="write"></a>
##### `write(self, unused_record)`


## Functions
<a name="convert"></a>
### `convert(in_filename, out_filename)`
```
Converts a recognized genomics file `in_filename` to `out_filename`.

Args:
  in_filename: str; filename of a genomics data file to use as input.
  out_filename: str; filename of a genomics data file to use as output, or
    None, if no output should be written.

Raises:
  ConversionError, if the conversion could not be executed.
```

<a name="main"></a>
### `main(argv)`


