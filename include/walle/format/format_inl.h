#ifndef WALLE_FORMAT_FORMAT_INL_H_
#define WALLE_FORMAT_FORMAT_INL_H_

#include <walle/format/format.h>
#ifndef WALLE_FORMAT_FORMAT_H_
#error "can not be include "
#endif
#include <string.h>

#include <cctype>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstddef>  // for std::ptrdiff_t
# include <cstring>

#if FMT_EXCEPTIONS
# define FMT_TRY try
# define FMT_CATCH(x) catch (x)
#else
# define FMT_TRY if (true)
# define FMT_CATCH(x) if (false)
#endif


// Dummy implementations of strerror_r and strerror_s called if corresponding
// system functions are not available.
WALLE_MAYBE_UNUSED
static inline walle::internal::Null<> strerror_r(int, char *, ...) {
  return walle::internal::Null<>();
}
WALLE_MAYBE_UNUSED
static inline walle::internal::Null<> strerror_s(char *, std::size_t, ...) {
  return walle::internal::Null<>();
}

namespace walle {

WALLE_FORCE_INLINE internal::RuntimeError::~RuntimeError() WALLE_NOEXCEPT {}
WALLE_FORCE_INLINE FormatError::~FormatError() WALLE_NOEXCEPT {}
WALLE_FORCE_INLINE SystemError::~SystemError() WALLE_NOEXCEPT {}

namespace {
# define FMT_SNPRINTF snprintf
# define FMT_SWPRINTF swprintf

const char RESET_COLOR[] = "\x1b[0m";

typedef void (*FormatFunc)(Writer &, int, StringRef);

// Portable thread-safe version of strerror.
// Sets buffer to point to a string describing the error code.
// This can be either a pointer to a string stored in buffer,
// or a pointer to some static immutable string.
// Returns one of the following values:
//   0      - success
//   ERANGE - buffer is not large enough to store the error message
//   other  - failure
// Buffer should be at least of size 1.
int safe_strerror(
    int error_code, char *&buffer, std::size_t buffer_size) WALLE_NOEXCEPT {
  WALLE_ASSERT(buffer != WALLE_NULL && buffer_size != 0, "invalid buffer");

  class StrError {
   private:
    int error_code_;
    char *&buffer_;
    std::size_t buffer_size_;

    // A noop assignment operator to avoid bogus warnings.
    void operator=(const StrError &) {}

    // Handle the result of XSI-compliant version of strerror_r.
    int handle(int result) {
      // glibc versions before 2.13 return result in errno.
      return result == -1 ? errno : result;
    }

    // Handle the result of GNU-specific version of strerror_r.
    int handle(char *message) {
      // If the buffer is full then the message is probably truncated.
      if (message == buffer_ && strlen(buffer_) == buffer_size_ - 1)
        return ERANGE;
      buffer_ = message;
      return 0;
    }

    // Handle the case when strerror_r is not available.
    int handle(internal::Null<>) {
      return fallback(strerror_s(buffer_, buffer_size_, error_code_));
    }

    // Fallback to strerror_s when strerror_r is not available.
    int fallback(int result) {
      // If the buffer is full then the message is probably truncated.
      return result == 0 && strlen(buffer_) == buffer_size_ - 1 ?
            ERANGE : result;
    }

#ifdef __c2__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

    // Fallback to strerror if strerror_r and strerror_s are not available.
    int fallback(internal::Null<>) {
      errno = 0;
      buffer_ = strerror(error_code_);
      return errno;
    }

#ifdef __c2__
# pragma clang diagnostic pop
#endif

   public:
    StrError(int err_code, char *&buf, std::size_t buf_size)
      : error_code_(err_code), buffer_(buf), buffer_size_(buf_size) {}

    int run() {
      return handle(strerror_r(error_code_, buffer_, buffer_size_));
    }
  };
  return StrError(error_code, buffer, buffer_size).run();
}

void format_error_code(Writer &out, int error_code,
                       StringRef message) WALLE_NOEXCEPT {
  // Report error code making sure that the output fits into
  // INLINE_BUFFER_SIZE to avoid dynamic memory allocation and potential
  // bad_alloc.
  out.clear();
  static const char SEP[] = ": ";
  static const char ERROR_STR[] = "error ";
  // Subtract 2 to account for terminating null characters in SEP and ERROR_STR.
  std::size_t error_code_size = sizeof(SEP) + sizeof(ERROR_STR) - 2;
  typedef internal::IntTraits<int>::MainType MainType;
  MainType abs_value = static_cast<MainType>(error_code);
  if (internal::is_negative(error_code)) {
    abs_value = 0 - abs_value;
    ++error_code_size;
  }
  error_code_size += internal::count_digits(abs_value);
  if (message.size() <= internal::INLINE_BUFFER_SIZE - error_code_size)
    out << message << SEP;
  out << ERROR_STR << error_code;
  assert(out.size() <= internal::INLINE_BUFFER_SIZE);
}

void report_error(FormatFunc func, int error_code,
                  StringRef message) WALLE_NOEXCEPT {
  MemoryWriter full_message;
  func(full_message, error_code, message);
  // Use Writer::data instead of Writer::c_str to avoid potential memory
  // allocation.
  std::fwrite(full_message.data(), full_message.size(), 1, stderr);
  std::fputc('\n', stderr);
}
}  // namespace

WALLE_FORCE_INLINE void SystemError::init(
    int err_code, CStringRef format_str, ArgList args) {
  error_code_ = err_code;
  MemoryWriter w;
  format_system_error(w, err_code, format(format_str, args));
  std::runtime_error &base = *this;
  base = std::runtime_error(w.str());
}

template <typename T>
int internal::CharTraits<char>::format_float(
    char *buffer, std::size_t size, const char *format,
    unsigned width, int precision, T value) {
  if (width == 0) {
    return precision < 0 ?
        FMT_SNPRINTF(buffer, size, format, value) :
        FMT_SNPRINTF(buffer, size, format, precision, value);
  }
  return precision < 0 ?
      FMT_SNPRINTF(buffer, size, format, width, value) :
      FMT_SNPRINTF(buffer, size, format, width, precision, value);
}

template <typename T>
int internal::CharTraits<wchar_t>::format_float(
    wchar_t *buffer, std::size_t size, const wchar_t *format,
    unsigned width, int precision, T value) {
  if (width == 0) {
    return precision < 0 ?
        FMT_SWPRINTF(buffer, size, format, value) :
        FMT_SWPRINTF(buffer, size, format, precision, value);
  }
  return precision < 0 ?
      FMT_SWPRINTF(buffer, size, format, width, value) :
      FMT_SWPRINTF(buffer, size, format, width, precision, value);
}

template <typename T>
const char internal::BasicData<T>::DIGITS[] =
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";

#define FMT_POWERS_OF_10(factor) \
  factor * 10, \
  factor * 100, \
  factor * 1000, \
  factor * 10000, \
  factor * 100000, \
  factor * 1000000, \
  factor * 10000000, \
  factor * 100000000, \
  factor * 1000000000

template <typename T>
const uint32_t internal::BasicData<T>::POWERS_OF_10_32[] = {
  0, FMT_POWERS_OF_10(1)
};

template <typename T>
const uint64_t internal::BasicData<T>::POWERS_OF_10_64[] = {
  0,
  FMT_POWERS_OF_10(1),
  FMT_POWERS_OF_10(ULongLong(1000000000)),
  // Multiply several constants instead of using a single long long constant
  // to avoid warnings about C++98 not supporting long long.
  ULongLong(1000000000) * ULongLong(1000000000) * 10
};

WALLE_FORCE_INLINE void internal::report_unknown_type(char code, const char *type) {
  (void)type;
  if (std::isprint(static_cast<unsigned char>(code))) {
    WALLE_THROW(FormatError(
        format("unknown format code '{}' for {}", code, type)));
  }
  WALLE_THROW(FormatError(
      format("unknown format code '\\x{:02x}' for {}",
        static_cast<unsigned>(code), type)));
}

WALLE_FORCE_INLINE void format_system_error(
    Writer &out, int error_code, StringRef message) WALLE_NOEXCEPT {
  FMT_TRY {
    walle::StackBuffer<char, internal::INLINE_BUFFER_SIZE> buffer;
    buffer.resize(internal::INLINE_BUFFER_SIZE);
    for (;;) {
      char *system_message = &buffer[0];
      int result = safe_strerror(error_code, system_message, buffer.size());
      if (result == 0) {
        out << message << ": " << system_message;
        return;
      }
      if (result != ERANGE)
        break;  // Can't get error message, report error code instead.
      buffer.resize(buffer.size() * 2);
    }
  } FMT_CATCH(...) {}
  walle::format_error_code(out, error_code, message);  // 'walle::' is for bcc32.
}

template <typename Char>
void internal::FixedBuffer<Char>::grow(std::size_t) {
  WALLE_THROW(std::runtime_error("buffer overflow"));
}

WALLE_FORCE_INLINE internal::Arg internal::FormatterBase::do_get_arg(
    unsigned arg_index, const char *&error) {
  internal::Arg arg = args_[arg_index];
  switch (arg.type) {
  case internal::Arg::NONE:
    error = "argument index out of range";
    break;
  case internal::Arg::NAMED_ARG:
    arg = *static_cast<const internal::Arg*>(arg.pointer);
    break;
  default:
    /*nothing*/;
  }
  return arg;
}

WALLE_FORCE_INLINE void report_system_error(
    int error_code, walle::StringRef message) WALLE_NOEXCEPT {
  // 'walle::' is for bcc32.
  report_error(format_system_error, error_code, message);
}

WALLE_FORCE_INLINE void print(std::FILE *f, CStringRef format_str, ArgList args) {
  MemoryWriter w;
  w.write(format_str, args);
  std::fwrite(w.data(), 1, w.size(), f);
}

WALLE_FORCE_INLINE void print(CStringRef format_str, ArgList args) {
  print(stdout, format_str, args);
}

WALLE_FORCE_INLINE void print_colored(Color c, CStringRef format, ArgList args) {
  char escape[] = "\x1b[30m";
  escape[3] = static_cast<char>('0' + c);
  std::fputs(escape, stdout);
  print(format, args);
  std::fputs(RESET_COLOR, stdout);
}


}  // namespace walle

#endif //WALLE_FORMAT_FORMAT_INL_H_