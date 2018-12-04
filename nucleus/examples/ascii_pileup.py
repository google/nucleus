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

"""Print an ASCII art pileup image."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys

from absl import app

from nucleus.io import sam
from nucleus.util import ranges

ANSI_BOLD = '\033[1m'
ANSI_RED = '\033[91m'
ANSI_OFF = '\033[0m'

# TODO(thomaswc): Also have this print out the reference sequence at the
# top if a reference fasta file is supplied.


def print_read(left_pos, start, highlight_position, seq):
  """Prints an ASCII representation of a sequence to stdout."""
  s = ' ' * (start - left_pos)
  i = highlight_position - start
  j = i + 1
  s += seq[:i] + ANSI_BOLD + ANSI_RED + seq[i:j] + ANSI_OFF + seq[j:]
  print(s)


def main(argv):
  if len(argv) != 3:
    print('Usage: {} <input_sam> <chromosome>:<position>'.format(argv[0]))
    sys.exit(-1)
  in_sam = argv[1]
  r = ranges.parse_literal(argv[2])
  position = r.start

  with sam.SamReader(in_sam) as sam_reader:
    reads = sam_reader.query(r)
    pos_seq_pairs = sorted(
        (read.alignment.position.position, read.aligned_sequence)
        for read in reads)
    if not pos_seq_pairs:
      print('No overlapping reads found for', argv[2])
      sys.exit(0)

    left_position = pos_seq_pairs[0][0]
    for start, seq in pos_seq_pairs:
      print_read(left_position, start, position, seq)


if __name__ == '__main__':
  app.run(main)
