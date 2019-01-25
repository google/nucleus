# nucleus.io.sharded_file_utils -- Utility functions for working with sharded files.
**Source code:** [nucleus/io/sharded_file_utils.py](https://github.com/google/nucleus/tree/master/nucleus/io/sharded_file_utils.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
A sharded file is a single conceptual file that is broken into a collection
of files to make parallelization easier.  A sharded file spec is like a
filename for a sharded file; the file spec "/some/path/prefix@200.txt"
says that the sharded file consists of 200 actual files that have names like
"/some/path/prefix-00000-of-00200.txt", "/some/path/prefix-00001-of-00200.txt",
etc.  This module contains functions for parsing, generating, detecting and
resolving sharded file specs.

## Classes overview
Name | Description
-----|------------
[`ShardError`](#sharderror) | An I/O error.

## Functions overview
Name | Description
-----|------------
[`generate_sharded_file_pattern`](#generate_sharded_file_pattern)`(basename, num_shards, suffix)` | Generate a sharded file pattern.
[`generate_sharded_filenames`](#generate_sharded_filenames)`(spec)` | Generate the list of filenames corresponding to the sharding path.
[`glob_list_sharded_file_patterns`](#glob_list_sharded_file_patterns)`(comma_separated_patterns, sep=',')` | Generate list of filenames corresponding to `comma_separated_patterns`.
[`is_sharded_file_spec`](#is_sharded_file_spec)`(spec)` | Returns True if spec is a sharded file specification.
[`maybe_generate_sharded_filenames`](#maybe_generate_sharded_filenames)`(filespec)` | Potentially expands sharded filespec into a list of paths.
[`normalize_to_sharded_file_pattern`](#normalize_to_sharded_file_pattern)`(spec_or_pattern)` | Take a sharding spec or sharding file pattern and return a sharded pattern.
[`parse_sharded_file_spec`](#parse_sharded_file_spec)`(spec)` | Parse a sharded file specification.
[`resolve_filespecs`](#resolve_filespecs)`(shard, *filespecs)` | Transforms potentially sharded filespecs into their paths for single shard.
[`sharded_filename`](#sharded_filename)`(spec, i)` | Gets a path appropriate for writing the ith file of a sharded spec.

## Classes
### ShardError
```
An I/O error.
```

## Functions
<a name="generate_sharded_file_pattern"></a>
### `generate_sharded_file_pattern(basename, num_shards, suffix)`
```
Generate a sharded file pattern.

Args:
  basename: str. The basename for the files.
  num_shards: int. The number of shards.
  suffix: str. The suffix if there is one or ''.
Returns:
  pattern:
```

<a name="generate_sharded_filenames"></a>
### `generate_sharded_filenames(spec)`
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

<a name="glob_list_sharded_file_patterns"></a>
### `glob_list_sharded_file_patterns(comma_separated_patterns, sep=',')`
```
Generate list of filenames corresponding to `comma_separated_patterns`.

Args:
  comma_separated_patterns: str. A pattern or a comma-separated list of
    patterns that represent file names.
  sep: char. Separator character.

Returns:
  List of filenames, sorted and dedupped.
```

<a name="is_sharded_file_spec"></a>
### `is_sharded_file_spec(spec)`
```
Returns True if spec is a sharded file specification.
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

<a name="normalize_to_sharded_file_pattern"></a>
### `normalize_to_sharded_file_pattern(spec_or_pattern)`
```
Take a sharding spec or sharding file pattern and return a sharded pattern.

The input can be a sharding spec(e.g '/some/file@10') or a sharded file
pattern (e.g. '/some/file-?????-of-00010)

Args:
  spec_or_pattern: str. A sharded file specification or sharded file pattern.

Returns:
  A sharded file pattern.
```

<a name="parse_sharded_file_spec"></a>
### `parse_sharded_file_spec(spec)`
```
Parse a sharded file specification.

Args:
  spec: str. The sharded file specification. A sharded file spec is one like
    'gs://some/file@200.txt'. Here, '@200' specifies the number of shards.

Returns:
  basename: str. The basename for the files.
  num_shards: int >= 0. The number of shards.
  suffix: str. The suffix if there is one, or '' if not.
Raises:
  ShardError: If the spec is not a valid sharded specification.
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
  A list. The first element is the number of shards, which is an int >= 1 when
  filespecs contains sharded paths and 0 if none do. All subsequent
  returned values follow the shard-specific paths for each filespec, in order.

Raises:
  ValueError: if any filespecs are inconsistent.
```

<a name="sharded_filename"></a>
### `sharded_filename(spec, i)`
```
Gets a path appropriate for writing the ith file of a sharded spec.
```

