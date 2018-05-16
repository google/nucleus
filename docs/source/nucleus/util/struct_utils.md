# nucleus.util.struct_utils -- Struct proto utilities.
**Source code:** [nucleus/util/struct_utils.py](https://github.com/google/nucleus/tree/master/nucleus/util/struct_utils.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
This class provides wrappers for conveniently interacting with protos defined
in struct.proto, mostly ListValue and Value objects. It should primarily be used
by variant_utils and variantcallutils rather than being used directly.

## Functions overview
Name | Description
-----|------------
[`add_bool_field`](#add_bool_field)`(field_map, field_name, value)` | Appends the given boolean value(s) to field_map[field_name].
[`add_int_field`](#add_int_field)`(field_map, field_name, value)` | Appends the given int value(s) to field_map[field_name].
[`add_number_field`](#add_number_field)`(field_map, field_name, value)` | Appends the given number value(s) to field_map[field_name].
[`add_string_field`](#add_string_field)`(field_map, field_name, value)` | Appends the given string value(s) to field_map[field_name].
[`get_bool_field`](#get_bool_field)`(field_map, field_name, is_single_field=False)` | Returns the bool value(s) stored in `field_map[field_name]`.
[`get_int_field`](#get_int_field)`(field_map, field_name, is_single_field=False)` | Returns the int value(s) stored in `field_map[field_name]`.
[`get_number_field`](#get_number_field)`(field_map, field_name, is_single_field=False)` | Returns the number value(s) stored in `field_map[field_name]`.
[`get_string_field`](#get_string_field)`(field_map, field_name, is_single_field=False)` | Returns the string value(s) stored in `field_map[field_name]`.
[`set_bool_field`](#set_bool_field)`(field_map, field_name, value)` | Sets field_map[field_name] with the given boolean value(s).
[`set_int_field`](#set_int_field)`(field_map, field_name, value)` | Sets field_map[field_name] with the given int value(s).
[`set_number_field`](#set_number_field)`(field_map, field_name, value)` | Sets field_map[field_name] with the given number value(s).
[`set_string_field`](#set_string_field)`(field_map, field_name, value)` | Sets field_map[field_name] with the given string value(s).

## Functions
<a name="add_bool_field"></a>
### `add_bool_field(field_map, field_name, value)`
```
Appends the given boolean value(s) to field_map[field_name].

Args:
  field_map: Map(str --> ListValue) to modify.
  field_name: str. The name of the field to append value to.
  value: The boolean value(s) to append to the field. This can be a single
    boolean or a list of booleans.
```

<a name="add_int_field"></a>
### `add_int_field(field_map, field_name, value)`
```
Appends the given int value(s) to field_map[field_name].

Args:
  field_map: Map(str --> ListValue) to modify.
  field_name: str. The name of the field to append value to.
  value: The int value(s) to append to the field. This can be a single
    int or a list of ints.
```

<a name="add_number_field"></a>
### `add_number_field(field_map, field_name, value)`
```
Appends the given number value(s) to field_map[field_name].

Args:
  field_map: Map(str --> ListValue) to modify.
  field_name: str. The name of the field to append value to.
  value: The number value(s) to append to the field. This can be a single
    number or a list of numbers.
```

<a name="add_string_field"></a>
### `add_string_field(field_map, field_name, value)`
```
Appends the given string value(s) to field_map[field_name].

Args:
  field_map: Map(str --> ListValue) to modify.
  field_name: str. The name of the field to append value to.
  value: The string value(s) to append to the field. This can be a single
    string or a list of strings.
```

<a name="get_bool_field"></a>
### `get_bool_field(field_map, field_name, is_single_field=False)`
```
Returns the bool value(s) stored in `field_map[field_name]`.

If the field_name is not present in field_map, the empty list is returned.

Args:
  field_map: Map(str --> ListValue) of interest.
  field_name: str. The name of the field to extract bool values from.
  is_single_field: bool. If True, return the first bool value stored (it
    should be the only one in the field). Otherwise, return the list of
    bools.

Returns:
  The bool value(s) stored in the field_map under this field_name.
```

<a name="get_int_field"></a>
### `get_int_field(field_map, field_name, is_single_field=False)`
```
Returns the int value(s) stored in `field_map[field_name]`.

If the field_name is not present in field_map, the empty list is returned.

Args:
  field_map: Map(str --> ListValue) of interest.
  field_name: str. The name of the field to extract int values from.
  is_single_field: bool. If True, return the first int value stored (it
    should be the only one in the field). Otherwise, return the list of
    ints.

Returns:
  The int value(s) stored in the field_map under this field_name.
```

<a name="get_number_field"></a>
### `get_number_field(field_map, field_name, is_single_field=False)`
```
Returns the number value(s) stored in `field_map[field_name]`.

If the field_name is not present in field_map, the empty list is returned.

Args:
  field_map: Map(str --> ListValue) of interest.
  field_name: str. The name of the field to extract number values from.
  is_single_field: bool. If True, return the first number value stored (it
    should be the only one in the field). Otherwise, return the list of
    numbers.

Returns:
  The number value(s) stored in the field_map under this field_name.
```

<a name="get_string_field"></a>
### `get_string_field(field_map, field_name, is_single_field=False)`
```
Returns the string value(s) stored in `field_map[field_name]`.

If the field_name is not present in field_map, the empty list is returned.

Args:
  field_map: Map(str --> ListValue) of interest.
  field_name: str. The name of the field to extract string values from.
  is_single_field: bool. If True, return the first string value stored (it
    should be the only one in the field). Otherwise, return the list of
    strings.

Returns:
  The string value(s) stored in the field_map under this field_name.
```

<a name="set_bool_field"></a>
### `set_bool_field(field_map, field_name, value)`
```
Sets field_map[field_name] with the given boolean value(s).

Args:
  field_map: Map(str --> ListValue) to modify.
  field_name: str. The name of the field to set.
  value: The boolean value(s) to set the field to. This can be a single
    boolean or a list of booleans.
```

<a name="set_int_field"></a>
### `set_int_field(field_map, field_name, value)`
```
Sets field_map[field_name] with the given int value(s).

Args:
  field_map: Map(str --> ListValue) to modify.
  field_name: str. The name of the field to set.
  value: The int value(s) to set the field to. This can be a single int
    or a list of ints.
```

<a name="set_number_field"></a>
### `set_number_field(field_map, field_name, value)`
```
Sets field_map[field_name] with the given number value(s).

Args:
  field_map: Map(str --> ListValue) to modify.
  field_name: str. The name of the field to set.
  value: The number value(s) to set the field to. This can be a single number
    or a list of numbers.
```

<a name="set_string_field"></a>
### `set_string_field(field_map, field_name, value)`
```
Sets field_map[field_name] with the given string value(s).

Args:
  field_map: Map(str --> ListValue) to modify.
  field_name: str. The name of the field to set.
  value: The int value(s) to set the field to. This can be a single string or
    a list of strings.
```

