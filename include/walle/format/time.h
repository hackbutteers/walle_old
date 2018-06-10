#ifndef WALLE_FORMAT_TIME_H_
#define WALLE_FORMAT_TIME_H_

#include <walle/format/format.h>
#include <ctime>

namespace walle {
template <typename ArgFormatter>
void format_arg(BasicFormatter<char, ArgFormatter> &f,
                const char *&format_str, const std::tm &tm) 
{
    if (*format_str == ':') {
        ++format_str;
    }
    const char *end = format_str;
    while (*end && *end != '}') {
        ++end;
    }
    if (*end != '}') {
        WALLE_THROW(FormatError("missing '}' in format string"));
    }
    walle::StackBuffer<char, internal::INLINE_BUFFER_SIZE> format;
    format.append(format_str, end + 1);
    format[format.size() - 1] = '\0';
    BasicBuffer<char> &buffer = f.writer().buffer();
    std::size_t start = buffer.size();
    for (;;) {
        std::size_t size = buffer.capacity() - start;
        std::size_t count = std::strftime(&buffer[start], size, &format[0], &tm);
        if (count != 0) {
          buffer.resize(start + count);
          break;
        }
        if (size >= format.size() * 256) {
            break;
        }
        const std::size_t MIN_GROWTH = 10;
        buffer.reserve(buffer.capacity() + (size > MIN_GROWTH ? size : MIN_GROWTH));
    }
    format_str = end + 1;
}

namespace internal{
inline Null<> localtime_r(...) { return Null<>(); }
inline Null<> localtime_s(...) { return Null<>(); }
inline Null<> gmtime_r(...) { return Null<>(); }
inline Null<> gmtime_s(...) { return Null<>(); }
}

// Thread-safe replacement for std::localtime
inline std::tm localtime(std::time_t time) 
{
    struct LocalTime {
      std::time_t time_;
      std::tm tm_;

      LocalTime(std::time_t t): time_(t) {}

      bool run() 
      {
          using namespace walle::internal;
          return handle(localtime_r(&time_, &tm_));
      }

      bool handle(std::tm *tm) { return tm != WALLE_NULL; }

      bool handle(internal::Null<>) 
      {
          using namespace walle::internal;
          return fallback(localtime_s(&tm_, &time_));
      }

      bool fallback(int res) { return res == 0; }

      bool fallback(internal::Null<>) 
      {
          using namespace walle::internal;
          std::tm *tm = std::localtime(&time_);
          if (tm) {
            tm_ = *tm;
          }
          return tm != WALLE_NULL;
      }
    };
    LocalTime lt(time);
      if (lt.run()) {
          return lt.tm_;
      }
    // Too big time values may be unsupported.
    WALLE_THROW(walle::FormatError("time_t value out of range"));
    return std::tm();
}

// Thread-safe replacement for std::gmtime
inline std::tm gmtime(std::time_t time) 
{
    struct GMTime {
        std::time_t time_;
        std::tm tm_;

        GMTime(std::time_t t): time_(t) {}

        bool run() 
        {
          using namespace walle::internal;
          return handle(gmtime_r(&time_, &tm_));
        }

        bool handle(std::tm *tm) { return tm != WALLE_NULL; }

        bool handle(internal::Null<>) 
        {
            using namespace walle::internal;
            return fallback(gmtime_s(&tm_, &time_));
        }

        bool fallback(int res) { return res == 0; }

        bool fallback(internal::Null<>) 
        {
            std::tm *tm = std::gmtime(&time_);
            if (tm != WALLE_NULL) {
              tm_ = *tm;
            }
            return tm != WALLE_NULL;
        }
    };
    GMTime gt(time);
    if (gt.run()) {
        return gt.tm_;
    }
    // Too big time values may be unsupported.
    WALLE_THROW(walle::FormatError("time_t value out of range"));
    return std::tm();
}
} //namespace walle


#endif  // WALLE_FORMAT_TIME_H_
