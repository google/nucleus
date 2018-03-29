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

#include "tensorflow/core/lib/random/simple_philox.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"

using tensorflow::uint64;

namespace nucleus {

// Helper class for randomly sampling a fraction of values.
//
// API is simple: only a fraction_to_keep calls to Keep() will return true.
//
// So keeping a 10% fraction of the values in a vector<int> x is:
//
// FractionalSampler sampler(0.10, rng);
// for( int v : x ) {
//  if (sampler.Keep()) {
//    ...
//  }
//
// This is a templated class so it can use any Random Number Generator (RNG)
// that (1) can be copied and (2) provides a RandDouble() function that emits
// a value between 0.0 and 1.0.
template<typename RNG>
class FractionalSampler {
 public:
  // Creates a new FractionalSampler that keeps fraction_to_keep elements on
  // average among N calls to Keep(). RNG is be a random number generator
  // object that can be copied into a member variable of this class.
  explicit FractionalSampler(double fraction_to_keep, const RNG& rng)
      : fraction_to_keep_(fraction_to_keep), rng_(rng) {
    CHECK_GE(fraction_to_keep, 0.0) << "Must be between 0.0 and 1.0";
    CHECK_LE(fraction_to_keep, 1.0) << "Must be between 0.0 and 1.0";
  }

  // Randomly return true approximately fraction_to_keep of the time.
  bool Keep() const { return rng_.RandDouble() <= fraction_to_keep_; }

  // Gets the fraction of elements that will be kept.
  double FractionKept() const { return fraction_to_keep_; }

 private:
  const double fraction_to_keep_;
  mutable RNG rng_;
};

class PhiloxFractionalSampler
    : public FractionalSampler<tensorflow::random::SimplePhilox> {
 public:
  // Creates a new PhiloxFractionalSampler that keeps fraction_to_keep elements
  // on average among N calls to Keep() using a PhiloxRandom initialized with
  // with random_seed.
  explicit PhiloxFractionalSampler(double fraction_to_keep, uint64 random_seed)
      : FractionalSampler(fraction_to_keep,
                          tensorflow::random::SimplePhilox(&rng_gen_)),
        rng_gen_(random_seed) {}

  // Fraction of non-variant sites to emit as DeepVariantCalls.
  mutable tensorflow::random::PhiloxRandom rng_gen_;
};

}  // namespace nucleus

#endif  // THIRD_PARTY_NUCLEUS_UTIL_SAMPLERS_H_
