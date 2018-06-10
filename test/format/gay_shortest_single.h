#ifndef TEST_GAY_SHORTEST_SINGLE_H_
#define TEST_GAY_SHORTEST_SINGLE_H_
#include <walle/buffer/buffer_ref.h>
namespace walle{
namespace format_detail {

struct PrecomputedShortestSingle {
  float v;
  const char* representation;
  int decimal_point;
};

walle::BufferRef<const PrecomputedShortestSingle>
    precomputed_shortest_single_representations();

}  // namespace format_detail
}  //namespace walle

#endif  // TEST_GAY_SHORTEST_SINGLE_H_
