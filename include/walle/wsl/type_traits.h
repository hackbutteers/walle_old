#ifndef WALLE_WSL_TYPE_TRAITS_H_
#define WALLE_WSL_TYPE_TRAITS_H_
#include <walle/config/base.h>
#include <type_traits>
#include <utility>

namespace wsl {

template <typename T>
struct identity {
  typedef T type;
};

template <typename T>
using identity_t = typename identity<T>::type;

template <size_t Index>
struct static_index {
    static WALLE_CONSTEXPR size_t index = Index;

    operator size_t () const { return index; }
};

template <typename T>
struct is_std_array : public std::false_type { };

template <typename T, size_t N>
struct is_std_array<std::array<T, N> >: public std::true_type { };

template <typename T>
struct is_std_pair : public std::false_type { };

template <typename S, typename T>
struct is_std_pair<std::pair<S, T> >: public std::true_type { };

}
#endif