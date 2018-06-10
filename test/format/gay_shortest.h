#ifndef TEST_GAY_SHORTEST_H_
#define TEST_GAY_SHORTEST_H_
#include <walle/buffer/buffer_ref.h>

namespace walle {
namespace format_detail{
struct PrecomputedShortest {
  double v;
  const char* representation;
  int decimal_point;
};

walle::BufferRef<const PrecomputedShortest> precomputed_shortest_representations();

}  // namespace format_detail
} //namespace walle
#endif  // TEST_GAY_SHORTEST_H_
