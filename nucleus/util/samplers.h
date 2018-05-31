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

#ifndef THIRD_PARTY_NUCLEUS_UTIL_SAMPLERS_H_
#define THIRD_PARTY_NUCLEUS_UTIL_SAMPLERS_H_

#include <random>

#include "tensorflow/core/platform/logging.h"
#include "nucleus/platform/types.h"

namespace nucleus {

// Helper class for randomly sampling a fraction of values.
//
// API is simple: only a fraction_to_keep calls to Keep() will return true.
//
// So keeping a 10% fraction of the values in a vector<int> x is:
//
// FractionalSampler sampler(0.10, seed_uint);
// for( int v : x ) {
//  if (sampler.Keep()) {
//    ...
//  }
//
class FractionalSampler {
 public:
  // Creates a new FractionalSampler that keeps fraction_to_keep elements on
  // average among N calls to Keep().
  explicit FractionalSampler(double fraction_to_keep, uint64 random_seed)
      : fraction_to_keep_(fraction_to_keep),
        generator_(random_seed),
        uniform_(0.0, 1.0) {
    CHECK_GE(fraction_to_keep, 0.0) << "Must be between 0.0 and 1.0";
    CHECK_LE(fraction_to_keep, 1.0) << "Must be between 0.0 and 1.0";
  }

  // Randomly return true approximately fraction_to_keep of the time.
  bool Keep() const { return uniform_(generator_) <= fraction_to_keep_; }

  // Gets the fraction of elements that will be kept.
  double FractionKept() const { return fraction_to_keep_; }

 private:
  const double fraction_to_keep_;
  // Raw RNG, of a type compatible with STL distribution functions.
  mutable std::mt19937_64 generator_;
  // Distribution sampler, to sample uniformly from [0,1).
  mutable std::uniform_real_distribution<> uniform_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_UTIL_SAMPLERS_H_
