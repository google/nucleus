# nucleus.pip_package.setup -- Fake setup.py module for installing Nucleus.
**Source code:** [nucleus/pip_package/setup.py](https://github.com/google/nucleus/tree/master/nucleus/pip_package/setup.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
Usually, setup.py is invoked twice:  first, to build the pip package
and second to install it.

This setup.py is only used for installation; build_pip_package.sh is
used to create the package.  We do it this way because we need our
package to include symbolic links, which normal setup.py doesn't
support.

For the same reason, this setup.py is not implemented using setuptools.
Instead, we directly implement the four commands run by pip install
(https://pip.pypa.io/en/stable/reference/pip_install/#id46):
  * setup.py egg_info [--egg-base XXX]
  * setup.py install --record XXX [--single-version-externally-managed]
          [--root XXX] [--compile|--no-compile] [--install-headers XXX]
  * setup.py bdist_wheel -d XXX
  * setup.py clean

## Functions overview
Name | Description
-----|------------
[`copy_egg_info`](#copy_egg_info)`(dest_dir)` | Copies the .egg-info directory to the specified location.
[`find_destination`](#find_destination)`(is_user)` | Returns the directory we are supposed to install into.
[`main`](#main)`()` | 
[`touch`](#touch)`(fname)` | 

## Functions
<a name="copy_egg_info"></a>
### `copy_egg_info(dest_dir)`
```
Copies the .egg-info directory to the specified location.

Args:
  dest_dir: str. The destination directory.

Returns:
  0 on success, 1 on failure.
```

<a name="find_destination"></a>
### `find_destination(is_user)`
```
Returns the directory we are supposed to install into.
```

<a name="main"></a>
### `main()`


<a name="touch"></a>
### `touch(fname)`


