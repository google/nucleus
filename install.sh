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

NUCLEUS_BAZEL_VERSION="0.17.2"
NUCLEUS_TENSORFLOW_VERSION="1.11.0"

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
sudo -H apt-get -y install python-dev python-pip python-wheel
# pip 10.0 is broken, see https://github.com/pypa/pip/issues/5240
pip install --user --upgrade 'pip==9.0.3'

# Update PATH so that newly installed pip is the one we actually use.
export PATH="$HOME/.local/bin:$PATH"
echo "$(pip --version)"

# Install python packages used by Nucleus
################################################################################
pip install --user contextlib2
# sortedcontainers>=2.0.0 breaks intervaltree=2.1.0
# Remove this when https://github.com/chaimleib/intervaltree/pull/69
# is resolved.  Tracked internally at b/80085543.
pip install --user 'sortedcontainers==1.5.3'
pip install --user 'intervaltree==2.1.0'
pip install --user 'mock>=2.0.0'
pip install --user 'numpy==1.14'
pip install --user 'six>=1.11.0'
# These are required to build TensorFlow from source.
pip install --user 'keras_applications==1.0.6' --no-deps
pip install --user 'keras_preprocessing==1.0.5' --no-deps
pip install --user 'h5py==2.8.0'
pip install --user enum34
pip install --user 'protobuf==3.6.1'

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

if [[ -e /usr/local/clif/bin/pyclif ]];
then
  echo "CLIF already installed."
else
  # Figure out which linux installation we are on to fetch an appropriate
  # version of the pre-built CLIF binary. Note that we only support now Ubuntu
  # 14, Ubuntu 16, and Debian 9.
  case "$(lsb_release -d)" in
    *Ubuntu*16.*.*)  PLATFORM="ubuntu-16" ;;
    *Ubuntu*14.*.*)  PLATFORM="ubuntu-14" ;;
    *Debian*9.*)     PLATFORM="debian" ;;
    *Debian*rodete*) PLATFORM="debian" ;;
    *) echo "CLIF is not installed on this machine and a prebuilt binary is not
unavailable for this platform. Please install CLIF at
https://github.com/google/clif before continuing."
    exit 1
  esac

  PACKAGE_CURL_PATH="https://storage.googleapis.com/deepvariant/packages"
  OSS_CLIF_CURL_ROOT="${PACKAGE_CURL_PATH}/oss_clif"
  OSS_CLIF_PKG="oss_clif.${PLATFORM}.latest.tgz"

  if [[ ! -f "/tmp/${OSS_CLIF_PKG}" ]]; then
    curl "${OSS_CLIF_CURL_ROOT}/${OSS_CLIF_PKG}" > /tmp/${OSS_CLIF_PKG}
  fi

  (cd / && sudo tar xzf "/tmp/${OSS_CLIF_PKG}")
  sudo ldconfig  # Reload shared libraries.
fi

# Download and build TensorFlow
################################################################################
note_build_stage "Download and build TensorFlow"

if [[ ! -d ../tensorflow ]]; then
  note_build_stage "Cloning TensorFlow from github as ../tensorflow doesn't exist"
  (cd .. &&
   git clone https://github.com/tensorflow/tensorflow)
fi

(cd ../tensorflow &&
 git checkout v${NUCLEUS_TENSORFLOW_VERSION} &&
 echo | ./configure
 )

echo "Done installing prereqs at $(date)!"

if [[ "$1" != "--prereqs_only" ]]; then
  # Build Nucleus
  ################################################################################
  note_build_stage "Building Nucleus"

  COPT_FLAGS="--copt=-msse4.1 --copt=-msse4.2 --copt=-mavx --copt=-O3"
  bazel build -c opt ${COPT_FLAGS} nucleus/...

  bazel build :licenses_zip

fi

# Done!
################################################################################
echo "Installation complete at $(date)!"
