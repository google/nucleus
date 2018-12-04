/*
 * Copyright 2018 Google LLC.
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

#include <algorithm>
#include <cmath>

#include "nucleus/util/math.h"
#include "tensorflow/core/platform/logging.h"

namespace nucleus {

double PhredToPError(const int phred) {
  CHECK_GE(phred, 0);
  return std::pow(10.0, -static_cast<double>(phred) / 10.0);
}

double PhredToLog10PError(const int phred) {
  CHECK_GE(phred, 0);
  return -static_cast<double>(phred) / 10;
}

double PErrorToPhred(const double perror) {
  CHECK_GT(perror, 0);
  CHECK_LE(perror, 1);
  return Log10PErrorToPhred(PErrorToLog10PError(perror));
}

int PErrorToRoundedPhred(const double perror) {
  CHECK_GT(perror, 0);
  CHECK_LE(perror, 1);
  return Log10PErrorToRoundedPhred(PErrorToLog10PError(perror));
}

double PErrorToLog10PError(const double perror) {
  CHECK_GT(perror, 0);
  CHECK_LE(perror, 1);
  return std::log10(perror);
}

double Log10PErrorToPhred(const double log10_perror) {
  CHECK_LE(log10_perror, 0);
  return -10 * log10_perror;
}

int Log10PErrorToRoundedPhred(const double log10_perror) {
  return std::abs(std::round(Log10PErrorToPhred(log10_perror)));
}

double Log10PTrueToPhred(const double log10_ptrue,
                         const double value_if_not_finite) {
  const double ptrue = Log10ToReal(log10_ptrue);
  const double perror = std::log10(1 - ptrue);
  return std::isfinite(perror) ? -10 * perror : value_if_not_finite;
}

double Log10ToReal(const double log10_probability) {
  CHECK_LE(log10_probability, 0.0);
  return std::pow(10, log10_probability);
}

std::vector<double> ZeroShiftLikelihoods(
    const std::vector<double>& likelihoods) {
  std::vector<double> normalized(likelihoods.size());
  double max = *std::max_element(likelihoods.cbegin(), likelihoods.cend());
  std::transform(likelihoods.cbegin(), likelihoods.cend(),
                 normalized.begin(),
                 [max](double x) { return x - max; });
  return normalized;
}

}  // namespace nucleus
