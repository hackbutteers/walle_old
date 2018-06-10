#ifndef TEST_GAY_FIXED_H_
#define TEST_GAY_FIXED_H_
#include <walle/buffer/buffer_ref.h>

namespace walle {
namespace format_detail {
struct PrecomputedFixed {
  double v;
  int number_digits;
  const char* representation;
  int decimal_point;
};

// Returns precomputed values of dtoa. The strings have been generated using
// Gay's dtoa in mode "fixed".
walle::BufferRef<const PrecomputedFixed> precomputed_fixed_representations();

}  //namespace format_detail
}  //namespace walle 

#endif  // TEST_GAY_FIXED_H_
