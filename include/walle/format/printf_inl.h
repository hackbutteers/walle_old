/*
 Formatting library for C++

 Copyright (c) 2012 - 2016, Victor Zverovich
 All rights reserved.

 For the license information refer to format.h.
 */
#ifndef WALLE_FOMRAT_PRINTF_INL_H_
#define WALLE_FOMRAT_PRINTF_INL_H_

#include <walle/format/format.h>
#include <walle/format/printf.h>

#ifndef WALLE_FORMAT_PRINTF_H_
#error "can not be include"
#endif
namespace walle {

template <typename Char>
void printf(BasicWriter<Char> &w, BasicCStringRef<Char> format, ArgList args);

WALLE_FORCE_INLINE int fprintf(std::FILE *f, CStringRef format, ArgList args) {
  MemoryWriter w;
  printf(w, format, args);
  std::size_t size = w.size();
  return std::fwrite(w.data(), 1, size, f) < size ? -1 : static_cast<int>(size);
}

}  // namespace walle
#endif //WALLE_FOMRAT_PRINTF_INL_H_