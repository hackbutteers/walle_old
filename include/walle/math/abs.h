#ifndef WALLE_MATH_ABS_H_
#define WALLE_MATH_ABS_H_

namespace walle {
namespace math {

template <typename T>
T abs_diff(const T& a, const T& b) 
{
    return a > b ? a - b : b - a;
}
} //namespace math
} //namespace walle
#endif //WALLE_MATH_ABS_H_