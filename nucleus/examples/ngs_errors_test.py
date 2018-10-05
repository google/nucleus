# Copyright 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
r"""Tests for nucleus.examples.add_ad_to_vcf.

Generating the golden output:

./nucleus/examples/ngs_errors \
  --alsologtostderr \
  --ref nucleus/testdata/ucsc.hg19.chr20.unittest.fasta.gz \
  --vcf nucleus/testdata/test_nist.b37_chr20_100kbp_at_10mb.vcf.gz \
  --bam nucleus/testdata/NA12878_S1.chr20.10_10p1mb.bam \
  --examples_out nucleus/testdata/golden.examples.ngs_errors/ \
  --max_reads 100 \
  --model_dir /tmp/model
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import six

from absl import logging
from absl.testing import absltest
import numpy as np
import tensorflow as tf

from nucleus.examples import ngs_errors
from nucleus.io import genomics_reader
from nucleus.io import genomics_writer
from nucleus.testing import test_utils
from tensorflow.core.example import example_pb2
from tensorflow.python.training import session_run_hook


class LossExtractorHook(session_run_hook.SessionRunHook):
  """A session hook that extracts loss after each iteration."""

  def __init__(self):
    self.losses = []

  def after_run(self, run_context, run_values):
    losses = run_context.session.graph.get_collection('losses')
    self.losses += run_context.session.run(losses)


# pylint: enable=invalid-name
class NGSErrorsTest(absltest.TestCase):

  def _read_examples(self, train_test_val_split, path):
    """ Read examples from path.

    Args:
      train_test_val_split: a list of three floats that specify the relevant
        sizes of train, test and validation sets.
      path: the directory path where train, test and validation datasets can be
        found.

    Returns:
      A list of tf.Example.
    """
    train_examples = []
    eval_examples = []
    test_examples = []
    with genomics_reader.TFRecordReader(
        ngs_errors.get_train_filename(path), proto=example_pb2.Example) as fin:
      train_examples = list(fin)
    with genomics_reader.TFRecordReader(
        ngs_errors.get_eval_filename(path), proto=example_pb2.Example) as fin:
      eval_examples = list(fin)
    with genomics_reader.TFRecordReader(
        ngs_errors.get_test_filename(path), proto=example_pb2.Example) as fin:
      test_examples = list(fin)

    n_examples = len(train_examples) + len(eval_examples) + len(test_examples)
    return train_examples + eval_examples + test_examples

  def test_generate_tfrecords(self):
    examples_out = os.path.join(absltest.get_default_test_tmpdir(),
                                'examples_output')
    train_test_val_split = [0.7, 0.2, 0.1]
    ngs_read_length = ngs_errors.generate_tfrecord_datasets(
        train_test_val_split,
        ref_path=test_utils.genomics_core_testdata(
            'ucsc.hg19.chr20.unittest.fasta.gz'),
        vcf_path=test_utils.genomics_core_testdata(
            'test_nist.b37_chr20_100kbp_at_10mb.vcf.gz'),
        bam_path=test_utils.genomics_core_testdata(
            'NA12878_S1.chr20.10_10p1mb.bam'),
        out_dir=examples_out,
        max_reads=100)

    actual_examples = self._read_examples(train_test_val_split, examples_out)
    golden_examples = self._read_examples(
        train_test_val_split,
        test_utils.genomics_core_testdata('golden.examples.ngs_errors'))
    self.assertEqual(len(actual_examples), len(golden_examples))

    matched_examples = []
    for expected in golden_examples:
      for actual in actual_examples:
        if all(actual.features.feature[key] == expected.features.feature[key]
               for key in expected.features.feature.keys()):
          matched_examples.append(expected)
    self.assertEqual(golden_examples, matched_examples)

  def test_get_input_fn(self):
    test_file = test_utils.test_tmpfile('test.tfrecord')

    # Use a simple test example that consists of a read sequence of length 5.
    example = example_pb2.Example()
    read_sequence = 'ACGTA'
    true_sequence = 'ACCTA'
    aligned_qualities = [30, 30, 20, 30, 30]
    features = example.features
    features.feature['read_name'].bytes_list.value.append(six.b('test_seq'))
    features.feature['read_sequence'].int64_list.value.extend(
        ['ACGT'.index(b) for b in read_sequence])
    features.feature['read_qualities'].int64_list.value.extend(
        aligned_qualities)
    features.feature['true_sequence'].int64_list.value.extend(
        ['ACGT'.index(b) for b in true_sequence])
    features.feature['ref_match'].int64_list.value.extend([1, 1, 0, 1, 1])

    with genomics_writer.TFRecordWriter(test_file) as writer:
      writer.write(example)

    features, label = ngs_errors.get_input_fn(
        test_file,
        ngs_read_length=len(read_sequence),
        batch_size=1,
        num_epochs=1)()
    with tf.Session() as sess:
      features_val, label_val = sess.run([features, label])
      features_array = np.array(features_val)
      self.assertEqual((1, 4, 5, 3), features_array.shape)
      self.assertTrue(np.array_equal(np.array([[[1, 0, 0, 0, 1],
                                                [0, 1, 0, 0, 0],
                                                [0, 0, 1, 0, 0],
                                                [0, 0, 0, 1, 0]]]),
                                     features_array[:, :, :, 0]))
      self.assertTrue(
          np.array_equal(np.array([[[1, 1, 0, 1, 1],
                                    [1, 1, 0, 1, 1],
                                    [1, 1, 0, 1, 1],
                                    [1, 1, 0, 1, 1]]]),
                         features_array[:, :, :, 1]))
      self.assertTrue(
          np.array_equal(np.array([[[30, 30, 20, 30, 30],
                                    [30, 30, 20, 30, 30],
                                    [30, 30, 20, 30, 30],
                                    [30, 30, 20, 30, 30]]]),
                         features_array[:, :, :, 2]))
      self.assertTrue(np.array_equal(np.array([[0, 1, 1, 3, 0]]),
                                     np.array(label_val)))


  def test_get_model_fn(self):
    model_dir = os.path.join(absltest.get_default_test_tmpdir(), 'model')
    ngs_read_length = 101
    ngs_estimator = tf.estimator.Estimator(
        model_fn=ngs_errors.get_model_fn(
            learning_rate=0.0001, ngs_read_length=ngs_read_length),
        model_dir=model_dir)

    examples_out = test_utils.genomics_core_testdata(
        'golden.examples.ngs_errors')

    # This should run quickly because there are only 100 reads/examples in
    # golden.examples.ngs_errors.

    # Run train step twice to make sure loss is decreasing.
    hooks = [LossExtractorHook(), LossExtractorHook()]

    for i in range(2):
      train_spec = tf.estimator.TrainSpec(
          input_fn=ngs_errors.get_input_fn(
              ngs_errors.get_train_filename(examples_out),
              ngs_read_length,
              batch_size=10,
              num_epochs=1),
          max_steps=10,
          hooks=[hooks[i]])
      eval_spec = tf.estimator.EvalSpec(
          input_fn=ngs_errors.get_input_fn(
              ngs_errors.get_eval_filename(examples_out),
              ngs_read_length,
              batch_size=10,
              num_epochs=1),
          steps=None,
          start_delay_secs=0)
      tf.estimator.train_and_evaluate(ngs_estimator, train_spec, eval_spec)

    logging.info('losses for 1st iteration: %s', hooks[0].losses)
    logging.info('losses for 2nd iteration: %s', hooks[1].losses)
    # If the 2nd iteration stopped because of |max_steps| is reached,
    # hooks[1].losses will be empty.
    all_losses = hooks[0].losses + hooks[1].losses
    self.assertGreater(all_losses[0], all_losses[-1])

    predictions = ngs_estimator.evaluate(
        input_fn=ngs_errors.get_input_fn(
            ngs_errors.get_test_filename(examples_out),
            ngs_read_length,
            batch_size=1,
            num_epochs=1))

    # Make sure the accuracy on the test dataset is reasonable.
    self.assertGreater(predictions['accuracy'], 0.1)
    logging.info('Prediction on test data: accuracy: %f loss: %f',
                 predictions['accuracy'], predictions['loss'])


if __name__ == '__main__':
  absltest.main()
