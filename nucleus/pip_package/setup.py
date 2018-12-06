# Copyright 2018 Google LLC.
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
"""Setup module for turning Nucleus into a pip package.

Based on
https://github.com/pypa/sampleproject/blob/master/setup.py

This should be invoked through build_pip_package.sh, rather than run
directly.
"""

import fnmatch
import os

from setuptools import find_packages
from setuptools import setup

def find_files(pattern, root):
  """Return all the files matching pattern below root dir."""
  for dirpath, _, files in os.walk(root):
    for filename in fnmatch.filter(files, pattern):
      yield os.path.join(dirpath, filename)


def is_python_file(fn):
  return fn.endswith('.py') or fn.endswith('.pyc')


headers = list(find_files('*.h', 'nucleus'))

matches = ['../' + x for x in find_files('*', 'external')
           if not is_python_file(x)]

so_lib_paths = [
    i for i in os.listdir('.')
    if os.path.isdir(i) and fnmatch.fnmatch(i, '_solib_*')
]

for path in so_lib_paths:
  matches.extend(
      ['../' + x for x in find_files('*', path) if '.py' not in x]
  )

setup(
    name='google-nucleus',
    version='0.2.1',
    description='A library for reading and writing genomics data.',
    long_description=
"""
Nucleus is a library of Python and C++ code designed to make it easy to
read, write and analyze data in common genomics file formats like SAM and VCF.
In addition, Nucleus enables painless integration with the TensorFlow machine
learning framework, as anywhere a genomics file is consumed or produced, a
TensorFlow tfrecords file may be substituted.
""",
    url='https://github.com/google/nucleus',
    author='The Genomics team in Google Brain',
    author_email='opensource@google.com',
    license='Apache 2.0',

    # Taken from list of valid classifiers at
    # https://pypi.python.org/pypi?%3Aaction=list_classifiers
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Healthcare Industry',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: Apache Software License',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python',
        'Topic :: Scientific/Engineering :: Bio-Informatics',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],

    keywords='genomics tensorflow bioinformatics',

    packages=find_packages(exclude=['g3doc', 'testdata']),

    # TODO(thomaswc): Consider adding some version requirements to
    # these install_requires.
    install_requires=['contextlib2', 'intervaltree', 'absl-py',
                      'mock', 'numpy', 'six', 'tensorflow>=1.11.0'],

    headers=headers,

    include_package_data=True,
    package_data={'nucleus': matches},

    data_files=[],

    entry_points={},

    project_urls={
        'Source': 'https://github.com/google/nucleus',
        'Bug Reports': 'https://github.com/google/nucleus/issues',
    },

    zip_safe=False,
)


