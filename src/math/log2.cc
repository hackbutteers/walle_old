#include <walle/math/log2.h>

namespace walle {
namespace math {

unsigned integer_log2_floor(int i) 
{
    return integer_log2_floor_template(i);
}

unsigned integer_log2_floor(unsigned int i) 
{
    return integer_log2_floor_template(i);
}

unsigned integer_log2_floor(long i) 
{
    return integer_log2_floor_template(i);
}

unsigned integer_log2_floor(unsigned long i) 
{
    return integer_log2_floor_template(i);
}

unsigned integer_log2_floor(long long i) 
{
    return integer_log2_floor_template(i);
}

unsigned integer_log2_floor(unsigned long long i) 
{
    return integer_log2_floor_template(i);
}

unsigned integer_log2_ceil(int i) 
{
    if (i <= 1) return 0;
    return integer_log2_floor(i - 1) + 1;
}

unsigned integer_log2_ceil(unsigned int i) 
{
    if (i <= 1) return 0;
    return integer_log2_floor(i - 1) + 1;
}

unsigned integer_log2_ceil(long i) 
{
    if (i <= 1) return 0;
    return integer_log2_floor(i - 1) + 1;
}

unsigned integer_log2_ceil(unsigned long i) 
{
    if (i <= 1) return 0;
    return integer_log2_floor(i - 1) + 1;
}

unsigned integer_log2_ceil(long long i) 
{
    if (i <= 1) return 0;
    return integer_log2_floor(i - 1) + 1;
}

unsigned integer_log2_ceil(unsigned long long i) 
{
    if (i <= 1) return 0;
    return integer_log2_floor(i - 1) + 1;
}
}//namespace math
} //namespace walle