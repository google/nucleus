#!/bin/bash
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

# Usage:  ./nucleus/pip_package/build_pip_package.sh
#
# Important:  You must run
#   source install.sh
# before running this script.

set -e

bazel build -c opt $COPT_FLAGS nucleus/...

TMPDIR=$(mktemp -d -t tmp.XXXXXXXXXXX)

RUNFILES=bazel-bin/nucleus/pip_package/build_pip_package.runfiles/nucleus
cp -R "${RUNFILES}/nucleus" "${TMPDIR}"

so_lib_dir=$(ls $RUNFILES | grep solib) || true
if [ -n "${so_lib_dir}" ]; then
  mkdir "${TMPDIR}/${so_lib_dir}"
  cp -R "${RUNFILES}/${so_lib_dir}" "${TMPDIR}"
fi

# TODO(thomaswc): Check to see if we need to copy any headers out of
# protobuf_archive.

cp nucleus/pip_package/MANIFEST.in "${TMPDIR}"
cp README.md "${TMPDIR}"
cp nucleus/pip_package/setup.py "${TMPDIR}"

pushd "${TMPDIR}"
rm -f MANIFEST
echo $(date) : "=== Building wheel"
python setup.py bdist_wheel
popd
echo "Output wheel is in ${TMPDIR}/dist"
