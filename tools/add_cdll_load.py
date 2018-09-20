#!/usr/bin/python
# Copyright 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Adds a dynamic library load to the top of a Python program.

The only tricky part about this is that Python programs aren't allowed to
have any code before `from __future__` imports, so we have to insert our
load after all of those.

Also, this script is used by nucleus.bzl, so it can't be compiled or
otherwise be the output of a bazel build.

Usage:
  add_cdll_load.py input_program.py some_library.so output_program.py
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import re
import sys

if len(sys.argv) != 4:
  print('Usage: %s <input_program> <so_file> <output_program>' % sys.argv[0])
  sys.exit(-1)

in_py = sys.argv[1]
so_file = sys.argv[2]
out_py = sys.argv[3]

load_str = '\nimport ctypes\nctypes.CDLL(\"%s\", ctypes.RTLD_GLOBAL)\n' % (
    so_file)

with open(in_py, 'r') as in_file:
  in_str = in_file.read()
  last_future_re = re.compile(r'(.*)(^from __future__ import .+?$)(.*)',
                              re.MULTILINE | re.DOTALL)
  m = re.match(last_future_re, in_str)
  if m is not None:
    out_str = m.group(1) + m.group(2) + load_str + m.group(3)
  else:
    out_str = load_str + in_str

  with open(out_py, 'w') as out_file:
    out_file.write(out_str)
