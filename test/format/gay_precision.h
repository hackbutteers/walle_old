// Copyright 2006-2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
#ifndef TEST_GAY_PRECISION_H_
#define TEST_GAY_PRECISION_H_
#include <walle/buffer/buffer_ref.h>
namespace walle {
namespace format_detail {
struct PrecomputedPrecision {
  double v;
  int number_digits;
  const char* representation;
  int decimal_point;
};

// Returns precomputed values of dtoa. The strings have been generated using
// Gay's dtoa in mode "precision".
walle::BufferRef<const PrecomputedPrecision> precomputed_precision_representations();

}  // namespace format_detail
}  // namespace walle

#endif  // TEST_GAY_PRECISION_H_
