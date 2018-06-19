#ifndef WALLE_WSL_META_H_
#define WALLE_WSL_META_H_
#include <walle/wsl/type_traits.h>

namespace wsl {

template <size_t Index, size_t Size, typename Functor>
class call_for_range_impl
{
public:
    static void call(Functor&& f) {
        std::forward<Functor>(f)(StaticIndex<Index>());
        call_for_range_impl<Index + 1, Size - 1, Functor>::call(
            std::forward<Functor>(f));
    }
};

//! helper for call_for_range: base case
template <size_t Index, typename Functor>
class call_for_range_impl<Index, 0, Functor>
{
public:
    static void call(Functor&& /* f */) { }
};

template <size_t Size, typename Functor>
void call_for_range(Functor&& f) {
    call_for_range_impl<0, Size, Functor>::call(
        std::forward<Functor>(f));
}

//! Call a generic functor (like a generic lambda) for the integers [Begin,End).
template <size_t Begin, size_t End, typename Functor>
void call_for_range(Functor&& f) {
    call_for_range_impl<Begin, End - Begin, Functor>::call(
        std::forward<Functor>(f));
}

template <typename Functor, typename Arg>
void call_foreach_impl(Functor&& f, Arg&& arg) 
{
    std::forward<Functor>(f)(std::forward<Arg>(arg));
}

template <typename Functor, typename Arg, typename... MoreArgs>
void call_foreach_impl(
    Functor&& f, Arg&& arg, MoreArgs&& ... rest) 
{
    std::forward<Functor>(f)(std::forward<Arg>(arg));
    call_foreach_impl(
        std::forward<Functor>(f), std::forward<MoreArgs>(rest) ...);
}

template <typename Functor, typename... Args>
void call_foreach(Functor&& f, Args&& ... args) 
{
    call_foreach_impl(
        std::forward<Functor>(f), std::forward<Args>(args) ...);
}

}
#endif 