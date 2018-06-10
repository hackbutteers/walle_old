#include <walle/format/date_time_formatter.h>
#include <walle/format/date_time_format.h>

namespace walle {

TimeFormatter DateTimeFormatter::kHttpFormatter;

TimeFormatter DateTimeFormatter::kIso8601Formatter;

TimeFormatter  DateTimeFormatter::kIso8601FracFormatter;

TimeFormatter  DateTimeFormatter::kRfc822Formatter;

TimeFormatter  DateTimeFormatter::kRfc1123Formatter;

TimeFormatter  DateTimeFormatter::kRfc850Formatter;

TimeFormatter  DateTimeFormatter::kRfc1036Formatter;

TimeFormatter  DateTimeFormatter::kAscTimeFormatter;

TimeFormatter  DateTimeFormatter::kSortAbleFormatter;

class InitHelper{
public:
    InitHelper()
    {
        // DateTimeFormatter::kHttpFormatter.set_property("format", DateTimeFormat::HTTP_FORMAT);
        DateTimeFormatter::kHttpFormatter.set_property("format", "$0");
        DateTimeFormatter::kIso8601Formatter.set_property("format", "$1");
        DateTimeFormatter::kIso8601FracFormatter.set_property("format", "$2");
        DateTimeFormatter::kRfc822Formatter.set_property("format", "$3");
        DateTimeFormatter::kRfc1123Formatter.set_property("format", "$4");
        DateTimeFormatter::kRfc850Formatter.set_property("format", "$5");
        DateTimeFormatter::kRfc1036Formatter.set_property("format", "$6");
        DateTimeFormatter::kAscTimeFormatter.set_property("format", "$7");
        DateTimeFormatter::kSortAbleFormatter.set_property("format", "$8");   
    }
    ~InitHelper()
    {

    }
};

InitHelper helper;

}