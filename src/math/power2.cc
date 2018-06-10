#include <walle/math/power2.h>
#include <cstddef>

namespace walle {
namespace math {


template <typename Integral>
static inline bool is_power_of_two_template(Integral i) 
{
    if (i <= 0) 
        return false;
    return !(i & (i - 1));
}

bool is_power_of_two(int i) 
{
    return is_power_of_two_template(i);
}

bool is_power_of_two(unsigned int i) 
{
    return is_power_of_two_template(i);
}

bool is_power_of_two(long i) 
{
    return is_power_of_two_template(i);
}

bool is_power_of_two(unsigned long i) 
{
    return is_power_of_two_template(i);
}

bool is_power_of_two(long long i) 
{
    return is_power_of_two_template(i);
}

bool is_power_of_two(unsigned long long i) 
{
    return is_power_of_two_template(i);
}

template <typename Integral>
static inline Integral round_up_to_power_of_two_template(Integral n) 
{
    --n;
    for (size_t k = 1; k != 8 * sizeof(n); k <<= 1) {
        n |= n >> k;
    }
    ++n;
    return n;
}

int round_up_to_power_of_two(int i) 
{
    return round_up_to_power_of_two_template(i);
}

unsigned int round_up_to_power_of_two(unsigned int i) 
{
    return round_up_to_power_of_two_template(i);
}

long round_up_to_power_of_two(long i) 
{
    return round_up_to_power_of_two_template(i);
}

unsigned long round_up_to_power_of_two(unsigned long i) 
{
    return round_up_to_power_of_two_template(i);
}

long long round_up_to_power_of_two(long long i) 
{
    return round_up_to_power_of_two_template(i);
}

unsigned long long round_up_to_power_of_two(unsigned long long i) 
{
    return round_up_to_power_of_two_template(i);
}

int round_down_to_power_of_two(int i) 
{
    return round_up_to_power_of_two(i + 1) >> 1;
}

unsigned int round_down_to_power_of_two(unsigned int i) 
{
    return round_up_to_power_of_two(i + 1) >> 1;
}

long round_down_to_power_of_two(long i) 
{
    return round_up_to_power_of_two(i + 1) >> 1;
}

unsigned long round_down_to_power_of_two(unsigned long i) 
{
    return round_up_to_power_of_two(i + 1) >> 1;
}

long long round_down_to_power_of_two(long long i) 
{
    return round_up_to_power_of_two(i + 1) >> 1;
}

unsigned long long round_down_to_power_of_two(unsigned long long i) 
{
    return round_up_to_power_of_two(i + 1) >> 1;
}

}
}