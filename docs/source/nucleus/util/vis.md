# nucleus.util.vis -- Utility functions for visualization and inspection of pileup examples.
**Source code:** [nucleus/util/vis.py](https://github.com/google/nucleus/tree/master/nucleus/util/vis.py)

**Documentation index:** [doc_index.md](../../doc_index.md)

---
Visualization and inspection utility functions enable showing image-like array
data including those used in DeepVariant.

## Functions overview
Name | Description
-----|------------
[`add_header`](#add_header)`(img, labels, mark_midpoints=True, header_height=20)` | Adds labels to the image, evenly distributed across the top.
[`alt_allele_indices_from_example`](#alt_allele_indices_from_example)`(example)` | Extract indices of the particular alt allele(s) the example represents.
[`alt_bases_from_indices`](#alt_bases_from_indices)`(alt_allele_indices, alternate_bases)` | Get alt allele bases based on their indices.
[`alt_from_example`](#alt_from_example)`(example)` | Get alt allele(s) from a DeepVariant example.
[`array_to_png`](#array_to_png)`(arr, path=None, show=True, vmin=None, vmax=None, scale=None, labels=None)` | Save an array as a PNG image with PIL and show it.
[`autoscale_colors_for_png`](#autoscale_colors_for_png)`(arr, vmin=None, vmax=None)` | Adjust an array to prepare it for saving to an image.
[`channels_from_example`](#channels_from_example)`(example)` | Extract image from an Example and return the list of channels.
[`convert_6_channels_to_rgb`](#convert_6_channels_to_rgb)`(channels)` | Convert 6-channel image from DeepVariant to RGB for quick visualization.
[`draw_deepvariant_pileup`](#draw_deepvariant_pileup)`(example=None, channels=None, composite_type=None, annotated=True, labels=None, path=None, show=True, scale=None)` | Quick utility for showing a pileup example as channels or RGB.
[`get_image_array_from_example`](#get_image_array_from_example)`(example)` | Decode image/encoded and image/shape of an Example into a numpy array.
[`locus_id_from_variant`](#locus_id_from_variant)`(variant)` | Create a locus ID of form "chr:pos_ref" from a Variant object.
[`locus_id_with_alt`](#locus_id_with_alt)`(example)` | Get complete locus ID from a DeepVariant example.
[`save_to_png`](#save_to_png)`(arr, path=None, image_mode=None, show=True, labels=None, scale=None)` | Make a PNG and show it from a numpy array of dtype=np.uint8.
[`scale_colors_for_png`](#scale_colors_for_png)`(arr, vmin=0, vmax=255)` | Scale an array to integers between 0 and 255 to prep it for a PNG image.
[`split_3d_array_into_channels`](#split_3d_array_into_channels)`(arr)` | Split 3D array into a list of 2D arrays.
[`variant_from_example`](#variant_from_example)`(example)` | Extract Variant object from the 'variant/encoded' feature of an Example.

## Functions
<a name="add_header"></a>
### `add_header(img, labels, mark_midpoints=True, header_height=20)`
```
Adds labels to the image, evenly distributed across the top.

This is primarily useful for showing the names of channels.

Args:
  img: A PIL Image.
  labels: list of strs. Labels for segments to write across the top.
  mark_midpoints: bool. Whether to add a small vertical line marking the
    center of each segment of the image.
  header_height: int. Height of the header in pixels.

Returns:
  A new PIL Image, taller than the original img and annotated.
```

<a name="alt_allele_indices_from_example"></a>
### `alt_allele_indices_from_example(example)`
```
Extract indices of the particular alt allele(s) the example represents.

Args:
  example: a DeepVariant make_examples output example.

Returns:
  list of indices.
```

<a name="alt_bases_from_indices"></a>
### `alt_bases_from_indices(alt_allele_indices, alternate_bases)`
```
Get alt allele bases based on their indices.

e.g. one alt allele: [0], ["C"] => "C"
or with two alt alleles: [0,2], ["C", "TT", "A"] => "C-A"

Args:
  alt_allele_indices: list of integers. Indices of the alt alleles for a
    particular example.
  alternate_bases: list of strings. All alternate alleles for the variant.

Returns:
  str. Alt allele(s) at the indices, joined by '-' if more than 1.
```

<a name="alt_from_example"></a>
### `alt_from_example(example)`
```
Get alt allele(s) from a DeepVariant example.

Args:
  example: a DeepVariant make_examples output example

Returns:
  str. The bases of the alt alleles, joined by a -.
```

<a name="array_to_png"></a>
### `array_to_png(arr, path=None, show=True, vmin=None, vmax=None, scale=None, labels=None)`
```
Save an array as a PNG image with PIL and show it.

Args:
  arr: numpy array. Should be 2-dimensional or 3-dimensional where the third
    dimension has 3 channels.
  path: str. Path for the image output. Default is /tmp/tmp.png for quickly
    showing the image in a notebook.
  show: bool. Whether to show the image using IPython utilities, only works in
    notebooks.
  vmin: number. Minimum data value, which will correspond to black in
    greyscale or lack of each color in RGB images. Default None takes the
    minimum of the data from arr.
  vmax: number. Maximum data value, which will correspond to white in
    greyscale or full presence of each color in RGB images. Default None takes
    the max of the data from arr.
  scale: integer. Number of pixels wide and tall to show each cell in the
    array. This sizes up the image while keeping exactly the same number of
    pixels for every cell in the array, preserving resolution and preventing
    any interpolation or overlapping of pixels. Default None adapts to the
    size of the image to multiply it up until a limit of 500 pixels, a
    convenient size for use in notebooks. If saving to a file for automated
    processing, scale=1 is recommended to keep output files small and simple
    while still retaining all the information content.
  labels: list of str. Labels to show across the top of the image.

Returns:
  None. Saves an image at path and optionally shows it with IPython.display.
```

<a name="autoscale_colors_for_png"></a>
### `autoscale_colors_for_png(arr, vmin=None, vmax=None)`
```
Adjust an array to prepare it for saving to an image.

Re-scale numbers in the input array to go from 0 to 255 to adapt them for a
PNG image.

Args:
  arr: numpy array. Should be 2-dimensional or 3-dimensional where the third
    dimension has 3 channels.
  vmin: number (float or int). Minimum data value, which will correspond to
    black in greyscale or lack of each color in RGB images. Default None takes
    the minimum of the data from arr.
  vmax: number (float or int). Maximum data value, which will correspond to
    white in greyscale or full presence of each color in RGB images. Default
    None takes the max of the data from arr.

Returns:
  (modified numpy array, image_mode)
```

<a name="channels_from_example"></a>
### `channels_from_example(example)`
```
Extract image from an Example and return the list of channels.

Args:
  example: a tensorflow Example containing features that include
    "image/encoded" and "image/shape"

Returns:
  list of 2D numpy arrays, one for each channel.
```

<a name="convert_6_channels_to_rgb"></a>
### `convert_6_channels_to_rgb(channels)`
```
Convert 6-channel image from DeepVariant to RGB for quick visualization.

The 6 channels are: "read base", "base quality", "mapping quality", "strand",
"supports variant", "base != reference".

Args:
  channels: a list of 6 numpy arrays.

Returns:
  3D numpy array of 3 colors (Red, green, blue).
```

<a name="draw_deepvariant_pileup"></a>
### `draw_deepvariant_pileup(example=None, channels=None, composite_type=None, annotated=True, labels=None, path=None, show=True, scale=None)`
```
Quick utility for showing a pileup example as channels or RGB.

Args:
  example: A tensorflow Example containing image/encoded and image/shape
    features. Will be parsed through channels_from_example. Ignored if
    channels are provided directly. Either example OR channels is required.
  channels: list of 2D arrays containing the data to draw. Either example OR
    channels is required.
  composite_type: str or None. Method for combining channels. One of
    [None,"RGB"].
  annotated: bool. Whether to add channel labels and mark midpoints.
  labels: list of str. Which labels to add to the image. If annotated=True,
    use default channels labels for DeepVariant.
  path: str. Output file path for saving as an image. If None, just show plot.
  show: bool. Whether to display the image for ipython notebooks. Set to False
    to prevent extra output when running in bulk.
  scale: integer. Multiplier to enlarge the image. Default: None, which will
    set it automatically for a human-readable size. Set to 1 for no scaling.

Returns:
  None. Saves an image at path and optionally shows it with IPython.display.
```

<a name="get_image_array_from_example"></a>
### `get_image_array_from_example(example)`
```
Decode image/encoded and image/shape of an Example into a numpy array.

Parse image/encoded and image/shape features from a tensorflow Example and
decode the image into that shape.

Args:
  example: a tensorflow Example containing features that include
    "image/encoded" and "image/shape"

Returns:
  numpy array of dtype np.uint8.
```

<a name="locus_id_from_variant"></a>
### `locus_id_from_variant(variant)`
```
Create a locus ID of form "chr:pos_ref" from a Variant object.

Args:
  variant: a nucleus variant.

Returns:
  str.
```

<a name="locus_id_with_alt"></a>
### `locus_id_with_alt(example)`
```
Get complete locus ID from a DeepVariant example.

Args:
  example: a DeepVariant make_examples output example

Returns:
  str in the form "chr:pos_ref_alt.
```

<a name="save_to_png"></a>
### `save_to_png(arr, path=None, image_mode=None, show=True, labels=None, scale=None)`
```
Make a PNG and show it from a numpy array of dtype=np.uint8.

Args:
  arr: numpy array. Input array to save.
  path: str. file path at which to save the image.
  image_mode: "RGB" or "L". Leave as default=None to choose based on image
    dimensions.
  show: bool. Whether to display the image using IPython (for notebooks).
  labels: list of str. Labels to show across the top of the image.
  scale: integer. Number of pixels wide and tall to show each cell in the
    array. This sizes up the image while keeping exactly the same number of
    pixels for every cell in the array, preserving resolution and preventing
    any interpolation or overlapping of pixels. Default None adapts to the
    size of the image to multiply it up until a limit of 500 pixels, a
    convenient size for use in notebooks. If saving to a file for automated
    processing, scale=1 is recommended to keep output files small and simple
    while still retaining all the information content.

Returns:
  None. Saves an image at path and optionally shows it with IPython.display.
```

<a name="scale_colors_for_png"></a>
### `scale_colors_for_png(arr, vmin=0, vmax=255)`
```
Scale an array to integers between 0 and 255 to prep it for a PNG image.

Args:
  arr: numpy array. Input array made up of integers or floats.
  vmin: number. Minimum data value to map to 0. Values below this will be
    clamped to this value and therefore become 0.
  vmax: number. Maximum data value to map to 255. Values above this will be
    clamped to this value and therefore become 255.

Returns:
  numpy array of dtype np.uint8 (integers between 0 and 255).
```

<a name="split_3d_array_into_channels"></a>
### `split_3d_array_into_channels(arr)`
```
Split 3D array into a list of 2D arrays.

e.g. given a numpy array of shape (100, 200, 6), return a list of 6 channels,
each with shape (100, 200).

Args:
  arr: a 3D numpy array.

Returns:
  list of 2D numpy arrays.
```

<a name="variant_from_example"></a>
### `variant_from_example(example)`
```
Extract Variant object from the 'variant/encoded' feature of an Example.

Args:
  example: a DeepVariant-style make_examples output example.

Returns:
  A Nucleus Variant.
```

