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

// This is a Python extension module that exists soley to load the
// C++ generated messages for Nucleus's protocol buffers BEFORE the
// Python code for those protocol buffers is imported.

#include <Python.h>

#include "nucleus/protos/bed.pb.h"
#include "nucleus/protos/cigar.pb.h"
#include "nucleus/protos/fasta.pb.h"
#include "nucleus/protos/fastq.pb.h"
#include "nucleus/protos/gff.pb.h"
#include "nucleus/protos/position.pb.h"
#include "nucleus/protos/range.pb.h"
#include "nucleus/protos/reads.pb.h"
#include "nucleus/protos/reference.pb.h"
#include "nucleus/protos/struct.pb.h"
#include "nucleus/protos/variants.pb.h"

static PyMethodDef load_descriptors_methods[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION > 2
static struct PyModuleDef load_descriptors_module = {
  PyModuleDef_HEAD_INIT,
  "lib_load_descriptors",
  NULL,
  -1,
  load_descriptors_methods,
  NULL,
  NULL,
  NULL,
  NULL
};
#endif  /* Python 3 */

PyMODINIT_FUNC
#if PY_MAJOR_VERSION > 2
PyInit_lib_load_descriptors(void)
#else
initlib_load_descriptors(void)
#endif
{
  nucleus::genomics::v1::BedRecord().descriptor();
  nucleus::genomics::v1::BedHeader().descriptor();
  nucleus::genomics::v1::BedReaderOptions().descriptor();
  nucleus::genomics::v1::BedWriterOptions().descriptor();
  nucleus::genomics::v1::CigarUnit().descriptor();
  nucleus::genomics::v1::FastaRecord().descriptor();
  nucleus::genomics::v1::FastaReaderOptions().descriptor();
  nucleus::genomics::v1::FastaWriterOptions().descriptor();
  nucleus::genomics::v1::FastqRecord().descriptor();
  nucleus::genomics::v1::FastqReaderOptions().descriptor();
  nucleus::genomics::v1::FastqWriterOptions().descriptor();
  nucleus::genomics::v1::GffRecord().descriptor();
  nucleus::genomics::v1::GffHeader().descriptor();
  nucleus::genomics::v1::GffReaderOptions().descriptor();
  nucleus::genomics::v1::GffWriterOptions().descriptor();
  nucleus::genomics::v1::Position().descriptor();
  nucleus::genomics::v1::Range().descriptor();
  nucleus::genomics::v1::LinearAlignment().descriptor();
  nucleus::genomics::v1::Read().descriptor();
  nucleus::genomics::v1::SamHeader().descriptor();
  nucleus::genomics::v1::ReadGroup().descriptor();
  nucleus::genomics::v1::Program().descriptor();
  nucleus::genomics::v1::SamReaderOptions().descriptor();
  nucleus::genomics::v1::ReadRequirements().descriptor();
  nucleus::genomics::v1::ContigInfo().descriptor();
  nucleus::genomics::v1::ReferenceSequence().descriptor();
  nucleus::genomics::v1::Struct().descriptor();
  nucleus::genomics::v1::Value().descriptor();
  nucleus::genomics::v1::ListValue().descriptor();
  nucleus::genomics::v1::Variant().descriptor();
  nucleus::genomics::v1::VariantCall().descriptor();
  nucleus::genomics::v1::VcfHeader().descriptor();
  nucleus::genomics::v1::VcfFilterInfo().descriptor();
  nucleus::genomics::v1::VcfInfo().descriptor();
  nucleus::genomics::v1::VcfFormatInfo().descriptor();
  nucleus::genomics::v1::VcfStructuredExtra().descriptor();
  nucleus::genomics::v1::VcfExtra().descriptor();
  nucleus::genomics::v1::VcfReaderOptions().descriptor();
  nucleus::genomics::v1::VcfWriterOptions().descriptor();

  PyObject *m;

#if PY_MAJOR_VERSION > 2
  m = PyModule_Create(&load_descriptors_module);
  if (m == NULL) {
    return NULL;
  }
#else
  m = Py_InitModule("lib_load_descriptors", load_descriptors_methods);
  if (m == NULL) {
      return;
  }
#endif

#if PY_MAJOR_VERSION > 2
  return m;
#endif
}
