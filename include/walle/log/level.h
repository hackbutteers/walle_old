#ifndef WALLE_LOG_COMMON_H_
#define WALLE_LOG_COMMON_H_
#include <walle/config/base.h>

namespace walle {
namespace log  {

enum class Level {
    eTrace = 0,
    eDebug = 1,
    eInfo = 2,
    eWarn = 3,
    eError = 4,
    eCritical = 5,
    eFatal = 6
};
static const char* kLevelName[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL", "FATAL"};
static const char* kShortLevelName[] =   {"T", "D", "I", "W", "E", "C", "F"};

WALLE_FORCE_INLINE const char* level_str(Level l)
{
    return kLevelName[static_cast<int>(l)];
}

WALLE_FORCE_INLINE const char* level_short_str(Level l)
{
    return kShortLevelName[static_cast<int>(l)];
}


} //namespace log
} //namespace walle
#endif //WALLE_LOG_COMMON_H_