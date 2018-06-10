/*
 Formatting library for C++ - std::ostream support

 Copyright (c) 2012 - 2016, Victor Zverovich
 All rights reserved.

 For the license information refer to format.h.
 */

#ifndef WALLE_FORMAT_OSTREAM_INL_H_
#define WALLE_FORMAT_OSTREAM_INL_H_

#include <walle/format/ostream.h>
#ifndef WALLE_FORMAT_OSTREAM_H_
#error "can not be include"
#endif
namespace walle {

namespace internal {
WALLE_FORCE_INLINE void write(std::ostream &os, Writer &w) {
  const char *data = w.data();
  typedef internal::MakeUnsigned<std::streamsize>::Type UnsignedStreamSize;
  UnsignedStreamSize size = w.size();
  UnsignedStreamSize max_size =
      internal::to_unsigned((std::numeric_limits<std::streamsize>::max)());
  do {
    UnsignedStreamSize n = size <= max_size ? size : max_size;
    os.write(data, static_cast<std::streamsize>(n));
    data += n;
    size -= n;
  } while (size != 0);
}
}

WALLE_FORCE_INLINE void print(std::ostream &os, CStringRef format_str, ArgList args) {
  MemoryWriter w;
  w.write(format_str, args);
  internal::write(os, w);
}
}  // namespace fmt
#endif //WALLE_FORMAT_OSTREAM_INL_H_