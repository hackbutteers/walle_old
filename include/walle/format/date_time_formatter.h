#ifndef WALLE_FORMAT_DATE_TIME_FORMATTER_H_
#define WALLE_FORMAT_DATE_TIME_FORMATTER_H_
#include <walle/format/time_formatter.h>
#include <walle/time/timestamp.h>
#include <string>

namespace walle {

class DateTimeFormatter {
public:

static TimeFormatter kHttpFormatter;

static TimeFormatter kIso8601Formatter;

static TimeFormatter kIso8601FracFormatter;

static TimeFormatter kRfc822Formatter;

static TimeFormatter kRfc1123Formatter;

static TimeFormatter kRfc850Formatter;

static TimeFormatter kRfc1036Formatter;

static TimeFormatter kAscTimeFormatter;

static TimeFormatter kSortAbleFormatter;

};
}
#endif //WALLE_FORMAT_DATE_TIME_FORMATTER_H_