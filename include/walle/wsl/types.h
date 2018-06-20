#ifndef WALLE_WSL_TYPES_H_
#define WALLE_WSL_TYPES_H_
#include <walle/config/base.h>
#include <cstring>

namespace wsl {

template <typename Dest, typename Source>
inline Dest bit_cast(const Source& source) 
{
    static_assert(sizeof(Dest) == sizeof(Source), "size mismatch");
    Dest dest;
    std::memcpy(&dest, &source, sizeof(dest));
    return dest;
}

template <class Dest, class Source>
inline Dest bit_cast(Source* source) 
{
    return bit_cast<Dest>(reinterpret_cast<uintptr_t>(source));
}

template <typename To>
inline To implicit_cast(typename wsl::identity<To>::value to) 
{
  return to;
}
}
#endif //WALLE_WSL_TYPES_H_
