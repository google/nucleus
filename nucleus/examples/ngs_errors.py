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
# pylint: disable=line-too-long
r"""Creates an ML model to learn the error process of a sequencer.

This program reads in aligned NGS reads from `--bam` and emits a TFRecord of
tf.Examples containing the observed bases and quality scores for each read as
well as the true genome sequence for that read, which are determined from the
reference genome (`--ref`) and the known variants for the sample sequenced in
the BAM (`--vcf`). For example, suppose we have a read in --bam:

    sequence: ACGT
    aligned at: chr20:10-15
    cigar: 4M

And that the reference genome at chr20:10-15 is ACCT. This program first checks
that `--vcf` file doesn't have any genetic variants in the region chr20:10-15.
If there are no variants there, the program emits a tf.Example containing:

    read_sequence - the bases of the read ('ACGT')
    true_sequence - the reference genome bases ('ACCT')
    as well as read_name, cigar, and read_qualities

When run over a BAM file, we generate one tf.Example for each read that
satisfies our read requirements (e.g., aligned, not a duplicate), that spans a
region of the genome without genetic variants in `--vcf`, and spans reference
bases containing only A, C, G, or T bases.

The emitted examples are then used to train a TensorFlow model to "error
correct" a read. Note that the learned model, though it achieves high accuracy,
is only for illustration purposes. It is an interesting technical / scientific
question what kind of model architecture would perform well here, and how
accurately it can learn to identify errors in the input reads.  There are many
extensions to this example program that would be important to add when
attempting to training a production-grade error model (see next for one
example).

NOTE: This program doesn't filter out reads that aren't in the confidently
determined region of the genome. A more sophisticated extension would also
accept a confident regions BED file and use ranges.RangeSet to only include
reads fully within the confident regions. This is left as an exercise for the
user.

## Prerequisites

a) Grab a slice of the BAM file via samtools. We don't need to index it because
   we are only iterating over the records, and don't need the `query()`
   functionality.

    samtools view -h \
      ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/RMNISTHS_30xdownsample.bam \
      20:10,000,000-10,100,000 \
      -o /tmp/NA12878_sliced.bam

b) Get the Genome in a Bottle truth set for NA12878 and index it with tabix.

    wget \
      ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/release/NA12878_HG001/latest/GRCh37/HG001_GRCh37_GIAB_highconf_CG-IllFB-IllGATKHC-Ion-10X-SOLID_CHROM1-X_v.3.3.2_highconf_PGandRTGphasetransfer.vcf.gz \
      -O /tmp/NA12878_calls.vcf.gz

    tabix /tmp/NA12878_calls.vcf.gz

c) Get the reference genome FASTA file from the DeepVariant testdata GCP bucket
   and index it with samtools.

    gsutil cp gs://deepvariant/case-study-testdata/hs37d5.fa.gz /tmp/hs37d5.fa.gz
    samtools faidx /tmp/hs37d5.fa.gz

## Build and run

    bazel build -c opt $COPT_FLAGS nucleus/examples:ngs_errors

    bazel-bin/nucleus/examples/ngs_errors \
      --alsologtostderr \
      --ref /tmp/hs37d5.fa.gz \
      --vcf /tmp/NA12878_calls.vcf.gz \
      --bam /tmp/NA12878_sliced.bam \
      --examples_out /tmp/examples
      --model_dir /tmp/ngs_model
"""
# pylint: enable=line-too-long

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl import app
from absl import flags
from absl import logging
import six
import random
import math
import os
import tensorflow as tf
import numpy as np

from nucleus.io import fasta
from nucleus.io import genomics_writer
from nucleus.io import sam
from nucleus.io import vcf
from nucleus.protos import reads_pb2
from nucleus.protos import cigar_pb2
from nucleus.util import cigar
from nucleus.util import ranges
from nucleus.util import utils
from tensorflow.core.example import example_pb2

FLAGS = flags.FLAGS

flags.DEFINE_string(
    'bam',
    None,
    'Path to an aligned SAM/BAM file (not necessarily indexed) containing '
    'reads to convert to tf.Examples.')
flags.DEFINE_string(
    'ref',
    None,
    'Path to an indexed FASTA file to use to get the reference sequence for '
    'the reads in --bam. Should be the same reference genome used to align the '
    'SAM/BAM file provided by --bam.')
flags.DEFINE_string(
    'vcf',
    None,
    'Path to an indexed VCF file containing variant calls for the sample in '
    '--bam. These variants are used to eliminate regions where the sample '
    'differs from the reference genome.')
flags.DEFINE_string(
    'examples_out', None,
    'Directory where we will write our TFRecord of tf.Examples.')
flags.DEFINE_string('model_dir', None,
                    'Directory to write out the trained TensorFlow model.')
flags.DEFINE_integer(
    'max_reads',
    None,
    'If provided, the maximum number of reads to convert to examples.')


_ALLOWED_CIGAR_OPS = frozenset([cigar.CHAR_TO_CIGAR_OPS[op] for op in 'MX=S'])
_ALLOWED_BASES = 'ACGT'

_TRAIN_RECORD = 'train.tfrecord'
_EVAL_RECORD = 'eval.tfrecord'
_TEST_RECORD = 'test.tfrecord'


def get_train_filename(dir_name):
  return os.path.join(dir_name, _TRAIN_RECORD)


def get_eval_filename(dir_name):
  return os.path.join(dir_name, _EVAL_RECORD)


def get_test_filename(dir_name):
  return os.path.join(dir_name, _TEST_RECORD)


def is_usable_training_example(read, variants, ref_bases):
  """Returns True if we can use read to make a training example.

  Args:
    read: nucleus.genomics.v1.Read proto.
    variants: list[nucleus.genomics.v1.Variant] protos. A list of variants
      overlapping read.
    ref_bases: str. The reference bases for read.

  Returns:
    True if read can be used to construct a high-quality training example, False
    otherwise.
  """
  if variants:
    # We cannot be certain that the true sequence of read is the bases from the
    # reference genome because there are overlapping variant calls, indicating
    # that the sample may differ from the reference sequence under this read.
    return False
  if not all(cigarunit.operation in _ALLOWED_CIGAR_OPS
             for cigarunit in read.alignment.cigar):
    # The read's alignment is too complex for us to use, so return False.
    return False
  # The bases aren't just one of the ACGT option, so return False.
  if any(base not in _ALLOWED_BASES for base in ref_bases):
    return False
  if any(base not in _ALLOWED_BASES for base in read.aligned_sequence):
    return False
  return True


def make_example(read, ref_bases):
  """Creates a tf.Example for read and ref_bases.

  Args:
    read: nucleus.genomics.v1.Read proto with cigar, fragment_name, and
      aligned_sequence.
    ref_bases: str. The reference bases for read.

  Returns:
    A tf.Example protobuf with the following features:
      read_name - for debugging convenience
      read_sequence - the bases observed by the instrument
      read_qualities - the quality scores emitted by the instrument, as
                       phred-scaled integer values.
      true_sequence - the "true" bases that should have been observed for this
                      read, as extracted from the reference genome.
      ref_match -  an array with 1 at the position where read_sequence and
                   true_sequence match and 0 elsewhere.
  """
  # Create our example proto.
  example = example_pb2.Example()

  # Sanity checks to make sure inputs are of the right lengths.
  assert len(read.aligned_sequence) == len(ref_bases)
  assert len(read.aligned_sequence) == len(read.aligned_quality)

  # Set the features in our Example.
  # Note that the str(...) calls are necessary because proto string fields are
  # unicode objects and we can only add bytes to the bytes_list.
  features = example.features
  features.feature['read_name'].bytes_list.value.append(
      six.b(str(read.fragment_name)))
  features.feature['read_sequence'].int64_list.value.extend(
      [_ALLOWED_BASES.index(b) for b in read.aligned_sequence])
  features.feature['read_qualities'].int64_list.value.extend(
      read.aligned_quality)
  features.feature['true_sequence'].int64_list.value.extend(
      [_ALLOWED_BASES.index(b) for b in ref_bases])
  ref_match = [b1 == b2 for b1, b2 in zip(read.aligned_sequence, ref_bases)]
  features.feature['ref_match'].int64_list.value.extend(ref_match)
  return example


def make_ngs_error_examples(ref_path, vcf_path, bam_path):
  """ Yields tf.Example for training a ML model.

  Each tf.Example contains
  relevant features aboout the ngs read.

  Args:
    ref_path: str. A path to an indexed fasta file.
    vcf_path: str. A path to an indexed VCF file.
    bam_path: str. A path to an SAM/BAM file.

  Yields:
    A tuple (example, ngs_read_length, has_error), where example is a
    tf.Example, ngs_read_length is the length of the read generated by the
    sequencer, and has_error is a boolean specifying whether the example
    contains a read error.
  """

  # Create a ref_reader backed by ref.
  ref_reader = fasta.IndexedFastaReader(ref_path)

  # Create a vcf_reader backed by vcf.
  vcf_reader = vcf.VcfReader(vcf_path)

  # Create a sam_reader backed by bam. Provide an empty ReadRequirements
  # proto to the reader so it enables standard filtering based on the default
  # values of ReadRequirements. Also explicitly allow the reader to access an
  # unindexed BAM, so only the iterate() function is enabled.
  read_requirements = reads_pb2.ReadRequirements()
  sam_reader = sam.SamReader(bam_path, read_requirements=read_requirements)

  # All our readers and writers are context managers, so use the `with`
  # construct to open all of the inputs/outputs and close them when we are done
  # looping over our reads.
  with ref_reader, vcf_reader, sam_reader:
    # Loop over the reads in our BAM file:
    for read in sam_reader.iterate():
      # Get the Range proto describing the chrom/start/stop spanned by our read.
      assert len(read.alignment.cigar) > 0
      first_cigar = read.alignment.cigar[0]
      # If the first cigar is a CLIP_SOFT, the start of sequence is the cigar
      # operation length before the alignment position.
      start = read.alignment.position.position
      if first_cigar.operation == cigar_pb2.CigarUnit.CLIP_SOFT:
        start -= first_cigar.operation_length
      read_range = ranges.make_range(read.alignment.position.reference_name,
                                     start, start + len(read.aligned_sequence))

      # Get all of the variants that overlap our read range.
      variants = list(vcf_reader.query(read_range))

      # Get the reference bases spanned by our read.
      ref_bases = ref_reader.query(read_range)

      # Check that we can use our read for generating an example.
      if is_usable_training_example(read, variants, ref_bases):
        # Convert read and ref_bases to a tf.Example with make_example.
        yield make_example(read, ref_bases), len(read.aligned_sequence), (
            read.aligned_sequence != ref_bases)


def generate_tfrecord_datasets(train_test_val_split, ref_path, vcf_path,
                               bam_path, out_dir, max_reads):
  """Generates train, testand validation TFRecords and writes them to disk.

  Args:
    train_test_val_split: tuple of three floats that sum up to 1. Specifies how
      the test data should be split for training, testing and validation.
    ref_path: str. A path to an indexed fasta file.
    vcf_path: str. A path to an indexed VCF file.
    bam_path: str. A path to an SAM/BAM file.
    out_dir: str. A directory path where examples will be written.
    max_reads: int or None. If not None, emits at most max_reads examples to
      out_dir.

  Returns:
    The length of ngs reads.

  Raises:
    ValueError: If the input data is malformatted.
  """
  assert len(train_test_val_split) == 3
  n_examples = 0
  n_examples_with_error = 0

  if not os.path.exists(out_dir):
    os.mkdir(out_dir)

  # Create our TFRecordWriters to write tf.Examples to file.

  ngs_read_length = None
  gw = genomics_writer
  with gw.TFRecordWriter(get_train_filename(out_dir)) as train_out, \
       gw.TFRecordWriter(get_test_filename(out_dir)) as test_out, \
       gw.TFRecordWriter(get_eval_filename(out_dir)) as eval_out:
    for example, read_length, has_error in make_ngs_error_examples(
        ref_path, vcf_path, bam_path):
      if ngs_read_length is None:
        ngs_read_length = read_length
      else:
        if ngs_read_length != read_length:
          raise ValueError('Inconsistent read lengths in input BAM')
      if max_reads is not None and n_examples >= max_reads:
        break
      n_examples += 1
      if has_error:
        n_examples_with_error += 1
      # Write examples out to three sets -- train, validation and test.
      r = random.random()
      if r < train_test_val_split[0]:
        train_out.write(example)
      elif r < train_test_val_split[0] + train_test_val_split[1]:
        test_out.write(example)
      else:
        eval_out.write(example)
    logging.info(
        'Number of examples: %d Number of examples containing errors: %d',
        n_examples, n_examples_with_error)
    return ngs_read_length


def create_model(input_shape, output_shape):
  """Creates an ML model for "error correcting" a ngs read with errors.

  Args:
    input_shape: A list of three elements, specifying the height, width and
      channel of the input tensor.
    output_shape: A list of two elements, specifying the height and width of the
      output tensor.

  Returns:
    A model that consists of two conv layers and one fully connected layer.
  """
  data_format = 'channels_last'
  # These are set arbitrarily, and you can tune them to see how they affect
  # the training.
  NUM_LAYER1_FILTERS = 10
  NUM_LAYER2_FILTERS = 10
  LAYER1_KERNEL_SIZE = 5
  LAYER2_KERNEL_SIZE = 5
  return tf.keras.Sequential([
      tf.keras.layers.Conv2D(
          NUM_LAYER1_FILTERS,
          LAYER1_KERNEL_SIZE,
          strides=(1, 1),
          padding='same',
          data_format=data_format,
          activation=tf.nn.relu,
          input_shape=input_shape),
      tf.keras.layers.Conv2D(
          NUM_LAYER2_FILTERS,
          LAYER2_KERNEL_SIZE,
          strides=(1, 1),
          padding='same',
          data_format=data_format,
          activation=tf.nn.relu),
      tf.keras.layers.Flatten(),
      tf.keras.layers.Dense(np.prod(output_shape), activation=None),
      tf.keras.layers.Reshape(target_shape=output_shape),
  ])


def get_input_fn(filename, ngs_read_length, batch_size, num_epochs):
  """Gets an input_fn that will then be used to in a TensorFlow EstimatorSpec.

  Args:
    filename: str. name of the file that includes the dataset.
    ngs_read_length: int. The length of the ngs read.
    batch_size: int. The size of the batch.
    num_epochs: int. Specifies the number of times the dataset should be
      repeated.

  Returns:
    A function that returns batches of training/validation/testing features and
    labels during training.
  """

  def _process_input(proto_string):
    proto_features = {
        'read_sequence': tf.FixedLenFeature([ngs_read_length], tf.int64),
        'true_sequence': tf.FixedLenFeature([ngs_read_length], tf.int64),
        'read_qualities': tf.FixedLenFeature([ngs_read_length], tf.int64),
        'ref_match': tf.FixedLenFeature([ngs_read_length], tf.int64)
    }
    parsed_features = tf.parse_single_example(proto_string, proto_features)

    read_sequence = parsed_features['read_sequence']

    true_sequence = parsed_features['true_sequence']

    ref_matches = parsed_features['ref_match']
    ref_matches = tf.cast(ref_matches, tf.float32)
    ref_matches = tf.ones([len(_ALLOWED_BASES), 1]) * ref_matches

    read_qualities = parsed_features['read_qualities']
    read_qualities = tf.cast(read_qualities, tf.float32)
    read_qualities = tf.ones([len(_ALLOWED_BASES), 1]) * read_qualities

    features = tf.stack([
        tf.one_hot(
            read_sequence, depth=len(_ALLOWED_BASES), axis=0, dtype=tf.float32),
        ref_matches, read_qualities
    ],
                        axis=2)
    label = true_sequence
    return features, label

  def _input_fn():
    # TODO: Considering shuffling the dataset to eliminate batch effects.
    ds = tf.data.TFRecordDataset(filename)
    ds = ds.map(_process_input)
    ds = ds.batch(batch_size).repeat(num_epochs)
    iterator = ds.make_one_shot_iterator()
    return iterator.get_next()

  return _input_fn


def get_model_fn(learning_rate, ngs_read_length):
  """ Gets a model_fn that will be used to construct a TensorFlow Estimator.

  Args:
     learning_rate: float. The learning rate to be used in training.
     ngs_read_length: int. The length of the read produced by the sequencer.

  Returns:
     A model_fn that will be used to construct a TensorFlow Estimator.
  """

  # Note that labels is None if mode is PREDICT.
  def _model_fn(features, labels, mode, params):
    # Creates a model that takes in a volume of (height * width * channel) and
    # emits an output of shape (height * width). Here because we are using 3
    # features (read_sequence, read_qualities and ref_match), there are 3
    # channels in the input shape.
    model = create_model(
        input_shape=[len(_ALLOWED_BASES), ngs_read_length, 3],
        output_shape=[ngs_read_length, len(_ALLOWED_BASES)])
    logits = model(features, training=(mode == tf.estimator.ModeKeys.TRAIN))
    classes = tf.argmax(logits, axis=-1)

    if mode == tf.estimator.ModeKeys.PREDICT:
      predictions = {
          'classes': classes,
          'probabilities': tf.nn.softmax(logits, axis=-1)
      }
      return tf.estimator.EstimatorSpec(
          mode=tf.estimator.ModeKeys.PREDICT, predictions=predictions)
    if mode == tf.estimator.ModeKeys.TRAIN:
      optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate)

      loss = tf.losses.sparse_softmax_cross_entropy(
          labels=labels, logits=logits)
      accuracy = tf.metrics.accuracy(labels=labels, predictions=classes)

      tf.summary.scalar('train_accuracy', accuracy[1])
      return tf.estimator.EstimatorSpec(
          mode=tf.estimator.ModeKeys.TRAIN,
          loss=loss,
          train_op=optimizer.minimize(loss,
                                      tf.train.get_or_create_global_step()))
    if mode == tf.estimator.ModeKeys.EVAL:
      loss = tf.losses.sparse_softmax_cross_entropy(
          labels=labels, logits=logits)
      accuracy = tf.metrics.accuracy(labels=labels, predictions=classes)
      tf.summary.scalar('eval_accuracy', accuracy[1])
      return tf.estimator.EstimatorSpec(
          mode=tf.estimator.ModeKeys.EVAL,
          loss=loss,
          eval_metric_ops={
              'accuracy':
                  tf.metrics.accuracy(labels=labels, predictions=classes),
          })

  return _model_fn


def main(argv):
  del argv  # Unused.

  # Specifies how tfrecords should be split for training, testing and
  # validation. Here we reserve 70% of the total data for training, 20% for
  # testing and 10% for validation.
  TRAIN_TEST_VAL_SPLIT = [0.7, 0.2, 0.1]
  # Generate tfrecord datasets and write them to disk. They will be used for
  # training, testing and validation.
  # TODO(xunjieli): Add a flag to skip creating files and to use existing ones.
  ngs_read_length = generate_tfrecord_datasets(
      TRAIN_TEST_VAL_SPLIT,
      ref_path=FLAGS.ref,
      vcf_path=FLAGS.vcf,
      bam_path=FLAGS.bam,
      out_dir=FLAGS.examples_out,
      max_reads=FLAGS.max_reads)

  # Hyperparams that can be tuned
  TRAINING_STEPS = 10000
  LEARNING_RATE = 0.0001
  NUM_EPOCHS_BETWEEN_EVAL = 2
  ACCURACY_THRESHOLD = 0.98
  BATCH_SIZE = 10

  ngs_estimator = tf.estimator.Estimator(
      model_fn=get_model_fn(LEARNING_RATE, ngs_read_length),
      model_dir=FLAGS.model_dir)

  # Train and evaluate model.
  # The training loss should be decreasing and after 10000 steps, you should be
  # able to see an accuracy of 98%.
  for _ in range(TRAINING_STEPS // NUM_EPOCHS_BETWEEN_EVAL):
    train_spec = tf.estimator.TrainSpec(
        input_fn=get_input_fn(
            get_train_filename(FLAGS.examples_out),
            ngs_read_length,
            batch_size=BATCH_SIZE,
            num_epochs=NUM_EPOCHS_BETWEEN_EVAL),
        max_steps=TRAINING_STEPS)
    eval_spec = tf.estimator.EvalSpec(
        input_fn=get_input_fn(
            get_eval_filename(FLAGS.examples_out),
            ngs_read_length,
            batch_size=BATCH_SIZE,
            num_epochs=1),
        steps=None)
    metrics, _ = tf.estimator.train_and_evaluate(ngs_estimator, train_spec,
                                                 eval_spec)
    # metrics is None if max_steps has been reached in which case the
    # train_and_evaluate will not be run.
    if not metrics:
      break
    logging.info('Training after %d steps: accuracy: %f loss: %f',
                 metrics['global_step'], metrics['accuracy'], metrics['loss'])
    if metrics['accuracy'] > ACCURACY_THRESHOLD:
      break

  # Predict using the trained model.
  # You should be able to see a ~98% accuracy on the test set.
  # TODO: could write predicted sequences to disk (e.g. in a BAM format).
  predictions = ngs_estimator.evaluate(
      input_fn=get_input_fn(
          get_test_filename(FLAGS.examples_out),
          ngs_read_length,
          batch_size=1,
          num_epochs=1))

  logging.info('Prediction on test data: accuracy: %f loss: %f',
               predictions['accuracy'], predictions['loss'])


if __name__ == '__main__':
  flags.mark_flags_as_required(
      ['ref', 'vcf', 'bam', 'examples_out', 'model_dir'])
  app.run(main)
