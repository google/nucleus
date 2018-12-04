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

"""Given new genotype priors, update the variant calls in a VCF or gVCF file."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import math

from absl import app
from absl import flags

from nucleus.io import vcf
from nucleus.util import variant_utils
from nucleus.util import variantcall_utils

FLAGS = flags.FLAGS

flags.DEFINE_string(
    'input_vcf', None,
    'Filename of the VCF file to read.')
flags.DEFINE_string(
    'output_vcf', None,
    'Filename of the VCF file to write to.')
flags.DEFINE_list(
    'genotype_priors', [],
    'List of genotype priors.  The length of this list should be the same '
    'as the length of the PL or GL field in the input_vcf.')
flags.DEFINE_float(
    'posterior_margin', 0.0,
    'If the difference between the top two posteriors is less than this, '
    'make no call (i.e., call it as "./.").')


def recall_variant(log_priors, variant):
  """Update the genotype calls in variant given the new genotype priors."""
  for call in variant.calls:
    if len(log_priors) != len(call.genotype_likelihood):
      continue

    posteriors = [x + y for x, y in zip(log_priors, call.genotype_likelihood)]
    sorted_posts = sorted(posteriors)
    highest_post = sorted_posts[-1]

    margin = 0
    if FLAGS.posterior_margin > 0.0 and len(log_priors) > 1:
      margin = math.pow(10.0, highest_post) - math.pow(10.0, sorted_posts[-2])

    ploidy = variantcall_utils.ploidy(call)
    if margin < FLAGS.posterior_margin:
      call.genotype[:] = [-1] * ploidy
    else:
      best_genotype = posteriors.index(highest_post)
      call.genotype[:] = (
          variant_utils.allele_indices_for_genotype_likelihood_index(
              best_genotype, ploidy=ploidy))


def main(argv):
  del argv

  priors = map(float, FLAGS.genotype_priors)
  sump = sum(priors)
  log_priors = [math.log10(x / sump) for x in priors]

  with vcf.VcfReader(FLAGS.input_vcf) as reader:
    with vcf.VcfWriter(FLAGS.output_vcf, header=reader.header) as writer:
      for variant in reader:
        recall_variant(log_priors, variant)
        # TODO(thomaswc): Also update the variant's quality.
        writer.write(variant)


if __name__ == '__main__':
  flags.mark_flags_as_required(['input_vcf', 'output_vcf'])
  app.run(main)
