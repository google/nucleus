#!/bin/bash
#
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
#
# Usage:  source install.sh
#
# This script installs all the packages required to build Nucleus, and
# then builds Nucleus.  If the flag "--prereqs_only" is passed, only
# the prereqs will be installed and the nucleus build will be skipped.
#
# This script will run as-is on Ubuntu 14, Ubuntu 16, and Debian 9 systems.
# On all other systems, you will need to first install CLIF by following the
# instructions at https://github.com/google/clif#installation
#
# We also assume that apt-get is already installed and available.

# ------------------------------------------------------------------------------
# Global setting for nucleus builds
# ------------------------------------------------------------------------------

NUCLEUS_BAZEL_VERSION="3.7.2"
NUCLEUS_TENSORFLOW_VERSION="2.5.0"

function note_build_stage {
  echo "========== [$(date)] Stage '${1}' starting"
}

# Update package list
################################################################################
note_build_stage "Update package list"
sudo -H apt-get -qq -y update

# Install generic dependencies
################################################################################
note_build_stage "Update misc. dependencies"
sudo -H apt-get -y install pkg-config zip g++ zlib1g-dev unzip curl git lsb-release

# Install htslib dependencies
################################################################################
note_build_stage "Install htslib dependencies"
sudo -H apt-get -y install libssl-dev libcurl4-openssl-dev liblz-dev libbz2-dev liblzma-dev

# Install pip
################################################################################
note_build_stage "Update pip"
sudo -H apt-get -y install python3-dev python3-pip python3-wheel python3-setuptools
sudo -H apt-get -y update
sudo -H apt-get -y install python-is-python3
# TensorFlow 2.0 requires pip >= 19.0
python3 -m pip install --user -U pip

# Update PATH so that newly installed pip is the one we actually use.
export PATH="$HOME/.local/bin:$PATH"
echo "$(pip --version)"

# Install python packages used by Nucleus
################################################################################
python3 -m pip install --user contextlib2
python3 -m pip install --user 'sortedcontainers==2.1.0'
python3 -m pip install --user 'intervaltree==3.0.2'
python3 -m pip install --user 'mock>=2.0.0'
python3 -m pip install --user 'numpy==1.19.2'
python3 -m pip install --user 'six>=1.15.0'
python3 -m pip install --user 'Pillow>=5.4.1'
python3 -m pip install --user 'ipython>=7.9.0'

# We need a recent version of setuptools, because pkg_resources is included in
# setuptools, and protobuf's __init__.py contains the line
# __import__('pkg_resources').declare_namespace(__name__)
# and only recent versions of setuptools correctly sort the namespace
# module's __path__ list when declare_namespace is called.
python3 -m pip install --user 'setuptools==49.6.0'

# These are required to build TensorFlow from source.
python3 -m pip install --user 'keras_preprocessing==1.1.2' --no-deps
python3 -m pip install --user 'h5py==2.10.0'
python3 -m pip install --user enum34
python3 -m pip install --user 'protobuf==3.13.0'

# Install Bazel
################################################################################

note_build_stage "Install bazel"

function ensure_wanted_bazel_version {
  local wanted_bazel_version=$1
  rm -rf ~/bazel
  mkdir ~/bazel

  if
    v=$(bazel --bazelrc=/dev/null --nomaster_bazelrc version) &&
    echo "$v" | awk -v b="$wanted_bazel_version" '/Build label/ { exit ($3 != b)}'
  then
    echo "Bazel ${wanted_bazel_version} already installed on the machine, not reinstalling"
  else
    pushd ~/bazel
    curl -L -O https://github.com/bazelbuild/bazel/releases/download/"${wanted_bazel_version}"/bazel-"${wanted_bazel_version}"-installer-linux-x86_64.sh
    chmod +x bazel-*.sh
    ./bazel-"${wanted_bazel_version}"-installer-linux-x86_64.sh --user
    rm bazel-"${wanted_bazel_version}"-installer-linux-x86_64.sh
    popd
  fi

  PATH="$HOME/bin:$PATH"
}

ensure_wanted_bazel_version "${NUCLEUS_BAZEL_VERSION}"

# Install CLIF
################################################################################

note_build_stage "Install CLIF binary"

if [[ -e /usr/local/bin/pyclif ]];
then
  echo "CLIF already installed."
else
  # Build clif binary from scratch. Might not be ideal because it installs a
  # bunch of dependencies, but this works fine when we used this in a Dockerfile
  # because we don't do build-prereq.sh in the final image.
  time sudo ./build_clif.sh
  # TODO(b/181283422): Figure out why these symbolic links are needed and see if
  #                    we can do this better.
  sudo mkdir -p /usr/clang/bin/
  sudo ln -sf /usr/local/bin/clif-matcher /usr/clang/bin/clif-matcher
  sudo mkdir -p /usr/local/clif/bin
  sudo ln -sf /usr/local/bin/pyclif* /usr/local/clif/bin/
  DIST_PACKAGES_DIR=$(python3 -c "import site; print(site.getsitepackages()[0])")
  sudo ln -sf ${DIST_PACKAGES_DIR}/clif/python /usr/local/clif/
fi

# Download and build TensorFlow
################################################################################
note_build_stage "Download and build TensorFlow"

if [[ ! -d ../tensorflow ]]; then
  note_build_stage "Cloning TensorFlow from github as ../tensorflow doesn't exist"
  (cd .. &&
   git clone https://github.com/tensorflow/tensorflow)
fi

# The version of Python used by TensorFlow gets exported as the Bazel
# repository @local_config_python, which is then used by clif.BUILD.
export PYTHON_BIN_PATH=`which python3`
(cd ../tensorflow &&
 git checkout v${NUCLEUS_TENSORFLOW_VERSION} &&
 echo | ./configure
 )

echo "Done installing prereqs at $(date)!"

if [[ "$#" -eq 0 ]] || [[ "$1" != "--prereqs_only" ]]; then
  # Build Nucleus
  ################################################################################
  note_build_stage "Building Nucleus"

  COPT_FLAGS="--copt=-msse4.1 --copt=-msse4.2 --copt=-mavx --copt=-O3"
  # N.B. The --experimental_build_setting_api had to be added on protobuf
  # upgrade to 3.9.2 to avoid error in bazel_skylib:
  #   "parameter 'build_setting' is experimental and thus unavailable with the
  #    current flags. It may be enabled by setting
  #    --experimental_build_setting_api"
  # Presumably it won't be needed at some later point when bazel_skylib is
  # upgraded again.
  BAZEL_FLAGS="${COPT_FLAGS} --experimental_build_setting_api"
  bazel build -c opt ${BAZEL_FLAGS} nucleus/...

  bazel build ${BAZEL_FLAGS} :licenses_zip
fi

# Done!
################################################################################
echo "Installation complete at $(date)!"
