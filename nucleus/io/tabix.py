# Copyright 2019 Google LLC.
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
"""Creates tabix indices for VCFs."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from nucleus.io.python import tabix_indexer


def build_index(path):
  """Builds a tabix index for VCF at the specified path."""
  tabix_indexer.tbx_index_build(path)


def build_csi_index(path, min_shift):
  """Builds a csi index for VCF at the specified path."""
  tabix_indexer.csi_index_build(path, min_shift)
