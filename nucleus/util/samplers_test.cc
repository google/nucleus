/*
 * Copyright 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "nucleus/util/samplers.h"

#include "nucleus/testing/test_utils.h"
#include "tensorflow/core/lib/random/simple_philox.h"

#include "tensorflow/core/platform/test.h"

namespace nucleus {

using ::testing::DoubleNear;

template<typename RNG>
void VerifySampler(const FractionalSampler<RNG>& sampler, double fraction) {
  int n_kept = 0;
  int n_trials = 1000000;
  for (int i = 0; i < n_trials; ++i) {
    if (sampler.Keep()) {
      n_kept++;
    }
  }
  const double actual_fraction = n_kept / (1.0 * n_trials);
  EXPECT_THAT(actual_fraction, DoubleNear(fraction, 0.001));
}

class FractionalSamplerTest : public ::testing::TestWithParam<double> {};

TEST_P(FractionalSamplerTest, TestFractionalSampler) {
  // Test that the fractional sampler produces approximately fraction * n_trials
  // Keep()=true values over a large number of trials.
  const double fraction = GetParam();
  tensorflow::random::PhiloxRandom gen(123456 /* random seed */);
  tensorflow::random::SimplePhilox rng(&gen);
  FractionalSampler<tensorflow::random::SimplePhilox> sampler(fraction, rng);
  VerifySampler(sampler, fraction);
}

TEST_P(FractionalSamplerTest, TestPhiloxFractionalSampler) {
  // Test that the fractional sampler produces approximately fraction * n_trials
  // Keep()=true values over a large number of trials.
  const double fraction = GetParam();
  PhiloxFractionalSampler sampler(fraction, 123456 /* random seed */);
  VerifySampler(sampler, fraction);
}


INSTANTIATE_TEST_CASE_P(FractionalSamplerTest1, FractionalSamplerTest,
                        ::testing::Values(0.9, 0.1, 0.01, 0.05));

}  // namespace nucleus
