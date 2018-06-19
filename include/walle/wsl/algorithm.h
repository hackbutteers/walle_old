#ifndef WALLE_WSL_ALGORITHM_H_
#define WALLE_WSL_ALGORITHM_H_
#include <walle/config/base.h>
#include <walle/wsl/type_traits.h>
#include <algorithm>
#include <iterator>
namespace wsl {


namespace algo_detail {

// Performs comparisons with operator==, similar to C++14's `std::equal_to<>`.
struct equal_to {
  template <typename T, typename U>
  bool operator()(const T& a, const U& b) const {
    return a == b;
  }
};

template <typename InputIter1, typename InputIter2, typename Pred>
bool equal_impl(InputIter1 first1, InputIter1 last1, InputIter2 first2,
               InputIter2 last2, Pred pred, std::input_iterator_tag,
               std::input_iterator_tag) 
{
  while (true) {
    if (first1 == last1) return first2 == last2;
    if (first2 == last2) return false;
    if (!pred(*first1, *first2)) return false;
    ++first1;
    ++first2;
  }
}

template <typename InputIter1, typename InputIter2, typename Pred>
bool equal_impl(InputIter1 first1, InputIter1 last1, InputIter2 first2,
               InputIter2 last2, Pred&& pred, std::random_access_iterator_tag,
               std::random_access_iterator_tag) 
{
    return (last1 - first1 == last2 - first2) &&
         std::equal(first1, last1, first2, std::forward<Pred>(pred));
}

// When we are using our own internal predicate that just applies operator==, we
// forward to the non-predicate form of std::equal. This enables an optimization
// in libstdc++ that can result in std::memcmp being used for integer types.
template <typename InputIter1, typename InputIter2>
bool equal_impl(InputIter1 first1, InputIter1 last1, InputIter2 first2,
               InputIter2 last2, algo_detail::equal_to /* unused */,
               std::random_access_iterator_tag,
               std::random_access_iterator_tag) 
{
    return (last1 - first1 == last2 - first2) &&
         std::equal(first1, last1, first2);
}

template <typename It>
It rotate_impl(It first, It middle, It last, std::true_type) 
{
  return std::rotate(first, middle, last);
}

template <typename It>
It rotate_impl(It first, It middle, It last, std::false_type) 
{
  std::rotate(first, middle, last);
    return std::next(first, std::distance(middle, last));
}

}  // namespace algo_detail


template <typename InputIter1, typename InputIter2, typename Pred>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2,
           InputIter2 last2, Pred&& pred) 
{
    return algo_detail::equal_impl(
        first1, last1, first2, last2, std::forward<Pred>(pred),
        typename std::iterator_traits<InputIter1>::iterator_category{},
        typename std::iterator_traits<InputIter2>::iterator_category{});
}

// Performs comparison of two ranges specified by pairs of iterators using
// operator==.
template <typename InputIter1, typename InputIter2>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2,
           InputIter2 last2) {
    return wsl::equal(first1, last1, first2, last2,
                     algo_detail::equal_to{});
}

template <typename InputIterator, typename EqualityComparable>
bool linear_search(InputIterator first, InputIterator last,
                   const EqualityComparable& value) {
    return std::find(first, last, value) != last;
}

template <typename ForwardIterator>
ForwardIterator rotate(ForwardIterator first, ForwardIterator middle,
                       ForwardIterator last) 
{
    return algo_detail::rotate_impl(
      first, middle, last,
      std::is_same<decltype(std::rotate(first, middle, last)),
                   ForwardIterator>());
}

template <typename T>
WALLE_FORCE_INLINE WALLE_CONSTEXPR 
T align_up(T v, T align) 
{
    return (v + align - 1) & ~(align - 1);
}

template <typename T>
WALLE_FORCE_INLINE WALLE_CONSTEXPR
T* align_up(T* v, size_t align) 
{
    static_assert(sizeof(T) == 1, "align byte pointers only");
    return reinterpret_cast<T*>(align_up(reinterpret_cast<uintptr_t>(v), align));
}

template <typename T>
WALLE_FORCE_INLINE WALLE_CONSTEXPR
T align_down(T v, T align) 
{
    return v & ~(align - 1);
}

template <typename T>
WALLE_FORCE_INLINE WALLE_CONSTEXPR
T* align_down(T* v, size_t align) 
{
    static_assert(sizeof(T) == 1, "align byte pointers only");
    return reinterpret_cast<T*>(align_down(reinterpret_cast<uintptr_t>(v), align));
}
}
#endif //WALLE_WSL_ALGORITHM_H_