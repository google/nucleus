# nucleus.util.sequence_utils -- Utility functions for manipulating DNA sequences.
**Source code:** [nucleus/util/sequence_utils.py](https://github.com/google/nucleus/tree/master/nucleus/util/sequence_utils.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---


## Classes overview
Name | Description
-----|------------
[`Error`](#error) | Base error class.

## Functions overview
Name | Description
-----|------------
[`reverse_complement`](#reverse_complement)`(sequence, complement_dict=None)` | Returns the reverse complement of a DNA sequence.

## Classes
### Error
```
Base error class.
```

## Functions
<a name="reverse_complement"></a>
### `reverse_complement(sequence, complement_dict=None)`
```
Returns the reverse complement of a DNA sequence.

By default this will successfully reverse complement sequences comprised
solely of A, C, G, and T letters. Other complement dictionaries can be
passed in for more permissive matching.

Args:
  sequence: str. The input sequence to reverse complement.
  complement_dict: dict[str, str]. The lookup dictionary holding the
    complement base pairs.

Returns:
  The reverse complement DNA sequence.

Raises:
  Error: The sequence contains letters not present in complement_dict.
```

