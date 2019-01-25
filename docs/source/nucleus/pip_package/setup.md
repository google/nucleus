# nucleus.pip_package.setup -- Setup module for turning Nucleus into a pip package.
**Source code:** [nucleus/pip_package/setup.py](https://github.com/google/nucleus/tree/master/nucleus/pip_package/setup.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
Based on
https://github.com/pypa/sampleproject/blob/master/setup.py

This should be invoked through build_pip_package.sh, rather than run
directly.

## Functions overview
Name | Description
-----|------------
[`find_files`](#find_files)`(pattern, root)` | Return all the files matching pattern below root dir.
[`is_python_file`](#is_python_file)`(fn)` | 

## Functions
<a name="find_files"></a>
### `find_files(pattern, root)`
```
Return all the files matching pattern below root dir.
```

<a name="is_python_file"></a>
### `is_python_file(fn)`


