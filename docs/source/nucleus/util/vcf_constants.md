# nucleus.util.vcf_constants -- Constants related to the VCF variant specification.
**Source code:** [nucleus/util/vcf_constants.py](https://github.com/google/nucleus/tree/master/nucleus/util/vcf_constants.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
See the full specification at https://samtools.github.io/hts-specs/VCFv4.3.pdf
for details.

## Functions overview
Name | Description
-----|------------
[`create_get_fn`](#create_get_fn)`(value_type, number)` | Returns a callable that extracts the typed information from a ListValue.
[`reserved_filter_field`](#reserved_filter_field)`(field_id)` | Returns the reserved FILTER field with the given ID.
[`reserved_format_field`](#reserved_format_field)`(field_id)` | Returns the reserved FORMAT field with the given ID.
[`reserved_format_field_get_fn`](#reserved_format_field_get_fn)`(field_name)` | Returns the callable that gets the proper field for the given field_name.
[`reserved_format_field_set_fn`](#reserved_format_field_set_fn)`(field_name)` | Returns the callable that sets the proper field for the given field_name.
[`reserved_info_field`](#reserved_info_field)`(field_id)` | Returns the reserved INFO field with the given ID.
[`reserved_info_field_get_fn`](#reserved_info_field_get_fn)`(field_name)` | Returns the callable that gets the proper field for the given field_name.
[`reserved_info_field_set_fn`](#reserved_info_field_set_fn)`(field_name)` | Returns the callable that sets the proper field for the given field_name.

## Functions
<a name="create_get_fn"></a>
### `create_get_fn(value_type, number)`
```
Returns a callable that extracts the typed information from a ListValue.

Args:
  value_type: str. The value type stored as defined in the VCF 4.3 spec.
  number: str. The number of entries of this value as defined in the VCF spec.

Returns:
  A callable that takes two inputs: A Map(str --> ListValue) and a string
  field name and returns the associated typed value(s). The return value is
  a list of typed values or a single typed value, depending on the expected
  number of values returned.
```

<a name="reserved_filter_field"></a>
### `reserved_filter_field(field_id)`
```
Returns the reserved FILTER field with the given ID.
```

<a name="reserved_format_field"></a>
### `reserved_format_field(field_id)`
```
Returns the reserved FORMAT field with the given ID.
```

<a name="reserved_format_field_get_fn"></a>
### `reserved_format_field_get_fn(field_name)`
```
Returns the callable that gets the proper field for the given field_name.

Args:
  field_name: str. The field name of the reserved FORMAT field (e.g. 'AD').

Returns:
  The callable that takes in a Map(str --> ListValue), and field name and
  returns the associated typed value(s).

Raises:
  ValueError: The field_name is not a known reserved FORMAT field.
```

<a name="reserved_format_field_set_fn"></a>
### `reserved_format_field_set_fn(field_name)`
```
Returns the callable that sets the proper field for the given field_name.

Args:
  field_name: str. The field name of the reserved FORMAT field (e.g. 'AD').

Returns:
  The callable that takes in a Map(str --> ListValue), field name, and value
  and modifies the map to populate the field_name entry with the given value.

Raises:
  ValueError: The field_name is not a known reserved FORMAT field.
```

<a name="reserved_info_field"></a>
### `reserved_info_field(field_id)`
```
Returns the reserved INFO field with the given ID.
```

<a name="reserved_info_field_get_fn"></a>
### `reserved_info_field_get_fn(field_name)`
```
Returns the callable that gets the proper field for the given field_name.

Args:
  field_name: str. The field name of the reserved INFO field (e.g. 'MQ').

Returns:
  The callable that takes in a Map(str --> ListValue), and field name and
  returns the associated typed value(s).

Raises:
  ValueError: The field_name is not a known reserved INFO field.
```

<a name="reserved_info_field_set_fn"></a>
### `reserved_info_field_set_fn(field_name)`
```
Returns the callable that sets the proper field for the given field_name.

Args:
  field_name: str. The field name of the reserved INFO field (e.g. 'MQ').

Returns:
  The callable that takes in a Map(str --> ListValue), field name, and value
  and modifies the map to populate the field_name entry with the given value.

Raises:
  ValueError: The field_name is not a known reserved INFO field.
```

