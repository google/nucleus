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

"""Prints a TFRecord file created by Nucleus.

Usage:
  print_tfrecord <filename> <proto_name>
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys

from absl import app

from google.protobuf import text_format
from nucleus.io import genomics_reader
from nucleus.protos import bed_pb2
from nucleus.protos import bedgraph_pb2
from nucleus.protos import fasta_pb2
from nucleus.protos import fastq_pb2
from nucleus.protos import gff_pb2
from nucleus.protos import reads_pb2
from nucleus.protos import variants_pb2
# pylint: disable=g-direct-tensorflow-import
from nucleus.protos import example_pb2

PROTO_DB = {
    'BedGraphRecord': bedgraph_pb2.BedGraphRecord,
    'BedRecord': bed_pb2.BedRecord,
    'FastaRecord': fasta_pb2.FastaRecord,
    'FastqRecord': fastq_pb2.FastqRecord,
    'GffRecord': gff_pb2.GffRecord,
    'Read': reads_pb2.Read,
    'Variant': variants_pb2.Variant,
    'Example': example_pb2.Example
}


def main(argv):
  if len(argv) != 3:
    print('Usage: {} <filename> <proto_name>\n'.format(argv[0]))
    sys.exit(-1)

  filename = argv[1]
  proto_name = argv[2]

  if proto_name not in PROTO_DB:
    print('Unknown protocol buffer name {}\n'.format(proto_name))
    print('Known names are: {}\n'.format(' '.join(PROTO_DB.keys())))
    sys.exit(-1)

  proto = PROTO_DB[proto_name]

  with genomics_reader.TFRecordReader(filename, proto=proto) as reader:
    for record in reader:
      print(text_format.MessageToString(record))


if __name__ == '__main__':
  app.run(main)
