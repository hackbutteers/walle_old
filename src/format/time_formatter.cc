#include <walle/format/time_formatter.h>
#include <walle/format/date_time_format.h>
#include <walle/time/time_zone.h>

namespace walle {
namespace format_detail {

//* %w - abbreviated weekday (Mon, Tue, ...)
class w_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out<<DateTimeFormat::WEEKDAY_SHORT_NAMES[dt.day_of_week()];
    }
};

//* %W - full weekday (Monday, Tuesday, ...)
class W_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out<<DateTimeFormat::WEEKDAY_NAMES[dt.day_of_week()];
    }
};

// * %b - abbreviated month (Jan, Feb, ...)
class b_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out<<DateTimeFormat::MONTH_SHORT_NAMES[dt.month() - 1];
    }
};

//* %B - full month (January, February, ...)
class B_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out<<DateTimeFormat::MONTH_NAMES[dt.month() - 1];
    }
};

// * %d - zero-padded day of month (01 .. 31)

class d_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}", dt.day());
    }
};
//* %e - day of month (1 .. 31)
class e_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}", dt.day());
    }
};

//* %f - space-padded day of month ( 1 .. 31)
class f_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:2}", dt.day());
    }
};

// * %m - zero-padded month (01 .. 12)
class m_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}", dt.month());
    }
};

//* %n - month (1 .. 12)
class n_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}", dt.month());
    }
};

//* %o - space-padded month ( 1 .. 12)
class o_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{: 2}", dt.month());
    }
};

//* %y - year without century (70)
class y_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}", dt.year() % 100);
    }
};

//* %Y - year with century (1970)
class Y_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}", dt.year());
    }
};

//* %H - hour (00 .. 23)
class H_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}", dt.hour());
    }
};

//* %h - hour (00 .. 12)
class h_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}", dt.hour_am_pm());
    }
};

// * %a - am/pm
class a_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}",dt.is_am() ? "am" : "pm");
    }
};
// * %A - AM/PM
class A_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}",dt.is_am() ? "AM" : "PM");
    }
};

//* %M - minute (00 .. 59)
class M_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}",dt.minute());
    }
};
// * %S - second (00 .. 59)
class S_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}",dt.second());
    }
};
// * %s - seconds and microseconds (equivalent to %S.%F)
class s_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:02}.{:06}",dt.second(), dt.microsecond() + dt.millisecond() * 1000);
    }
};
// * %i - millisecond (000 .. 999)
class i_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:03}", dt.millisecond());
    }
};

//* %c - centisecond (0 .. 9)
class c_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}", dt.millisecond()/100);
    }
};

//* %F - fractional seconds/microseconds (000000 - 999999)
class F_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{:06}", dt.second(), dt.microsecond() + dt.millisecond() * 1000);
    }
};

//%z - time zone differential in ISO 8601 format (Z or +NN.NN)
class z_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out<<get_local_tzd().z;
    }

    void format(walle::MemoryWriter &out, const DateTime &dt, int tzd)
    {
        WALLE_UNUSED(dt);
        if (tzd != walle::TimeZone::kUtc) {
		    if (tzd >= 0) {
			    out.write("+{:02}.{:02}", tzd/3600, (tzd%3600)/60);
            } else {
			    out.write("-{:02}.{:02}", -tzd/3600, (-tzd%3600)/60);
		    }
	    } else {
            out<<'Z';
        }
    }
};

//* %Z - time zone differential in RFC format (GMT or +NNNN)
class Z_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out<<get_local_tzd().Z;
    }

    void format(walle::MemoryWriter &out, const DateTime &dt, int tzd)
    {
        WALLE_UNUSED(dt);
        if (tzd != walle::TimeZone::kUtc) {
		    if (tzd >= 0) {
			    out.write("+{:02}{:02}", tzd/3600,(tzd%3600)/60);
		    } else {
			    out.write("-{:02}{:02}", -tzd/3600,(-tzd%3600)/60);
		    }		
	    } else {
            out<<"GMT";
        }
    }
};

class p_formatter : public flag_formatter {
public:
    p_formatter(char c) : _c(c)
    {

    }
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out<<_c;
    }
private:
    char _c;
};

class P_formatter : public flag_formatter {
public:
    P_formatter(const std::string &p) : _str(p)
    {

    }
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out<<_str;
    }
private:
    std::string _str;
};

/*
const std::string DateTimeFormat::ISO8601_FORMAT("%Y-%m-%dT%H:%M:%S%z");
const std::string DateTimeFormat::ISO8601_FRAC_FORMAT("%Y-%m-%dT%H:%M:%s%z");
const std::string DateTimeFormat::RFC822_FORMAT("%w, %e %b %y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC1123_FORMAT("%w, %e %b %Y %H:%M:%S %Z");
const std::string DateTimeFormat::HTTP_FORMAT("%w, %d %b %Y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC850_FORMAT("%W, %e-%b-%y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC1036_FORMAT("%W, %e %b %y %H:%M:%S %Z");
const std::string DateTimeFormat::ASCTIME_FORMAT("%w %b %f %H:%M:%S %Y");
const std::string DateTimeFormat::SORTABLE_FORMAT("%Y-%m-%d %H:%M:%S");
*/


class http_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}, {:02} {} {} {:02}:{:02}:{:02} {}",
                  DateTimeFormat::WEEKDAY_SHORT_NAMES[dt.day_of_week()],
                  dt.day(),
                  DateTimeFormat::MONTH_SHORT_NAMES[dt.month() - 1],
                  dt.year(),
                  dt.hour(),
                  dt.minute(),
                  dt.second(),
                  get_local_tzd().Z);
    }
};
//const std::string DateTimeFormat::ISO8601_FORMAT("%Y-%m-%dT%H:%M:%S%z");
class iso8601_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}-{:02}-{:02}T{:02}:{:02}:{:02} {}",
                dt.year(),
                dt.month(), 
                dt.day(),                 
                dt.hour(),
                dt.minute(),
                dt.second(),
                get_local_tzd().z);
    }
};

//const std::string DateTimeFormat::ISO8601_FRAC_FORMAT("%Y-%m-%dT%H:%M:%s%z");
class iso8601_frac_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}-{:02}-{:02}T{:02}:{:02}:{:02}.{:06} {}",
                dt.year(),
                dt.month(), 
                dt.day(),                 
                dt.hour(),
                dt.minute(),
                dt.second(), 
                dt.microsecond() + dt.millisecond() * 1000,
                get_local_tzd().z);
    }
};

//const std::string DateTimeFormat::RFC822_FORMAT("%w, %e %b %y %H:%M:%S %Z");
class rfc822_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}, {} {} {:02} {:02}:{:02}:{:02} {}",
                  DateTimeFormat::WEEKDAY_SHORT_NAMES[dt.day_of_week()],
                  dt.day(),
                  DateTimeFormat::MONTH_SHORT_NAMES[dt.month() - 1],
                  dt.year()%100,
                  dt.hour(),
                  dt.minute(),
                  dt.second(),
                  get_local_tzd().Z);
    }
};

//const std::string DateTimeFormat::RFC1123_FORMAT("%w, %e %b %Y %H:%M:%S %Z");
class rfc1123_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}, {} {} {} {:02}:{:02}:{:02} {}",
                  DateTimeFormat::WEEKDAY_SHORT_NAMES[dt.day_of_week()],
                  dt.day(),
                  DateTimeFormat::MONTH_SHORT_NAMES[dt.month() - 1],
                  dt.year(),
                  dt.hour(),
                  dt.minute(),
                  dt.second(),
                  get_local_tzd().Z);
    }
};
//const std::string DateTimeFormat::RFC850_FORMAT("%W, %e-%b-%y %H:%M:%S %Z");
class rfc850_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}, {}-{}-{:02} {:02}:{:02}:{:02} {}",
                  DateTimeFormat::WEEKDAY_NAMES[dt.day_of_week()],
                  dt.day(),
                  DateTimeFormat::MONTH_SHORT_NAMES[dt.month() - 1],
                  dt.year()%100,
                  dt.hour(),
                  dt.minute(),
                  dt.second(),
                  get_local_tzd().Z);
    }
};

//const std::string DateTimeFormat::RFC1036_FORMAT("%W, %e %b %y %H:%M:%S %Z");
class rfc1036_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}, {} {} {:02} {:02}:{:02}:{:02} {}",
                  DateTimeFormat::WEEKDAY_NAMES[dt.day_of_week()],
                  dt.day(),
                  DateTimeFormat::MONTH_SHORT_NAMES[dt.month() - 1],
                  dt.year()%100,
                  dt.hour(),
                  dt.minute(),
                  dt.second(),
                  get_local_tzd().Z);
    }
};

//const std::string DateTimeFormat::ASCTIME_FORMAT("%w %b %f %H:%M:%S %Y");
class asctime_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{} {} {} {:02}:{:02}:{:02} {}",
                  DateTimeFormat::WEEKDAY_SHORT_NAMES[dt.day_of_week()],
                  dt.month(),
                  dt.day(),
                  dt.hour(),
                  dt.minute(),
                  dt.second(),
                  dt.year());
    }
};

//const std::string DateTimeFormat::SORTABLE_FORMAT("%Y-%m-%d %H:%M:%S");
class sort_able_formatter : public flag_formatter {
public:
    void format(walle::MemoryWriter &out, const DateTime &dt)
    {
        out.write("{}-{}-{} {:02}:{:02}:{:02}",
                dt.year(),
                dt.month(),
                dt.day(),
                dt.hour(),
                dt.minute(),
                dt.second()
                );
    }
};
} //format_detail

flag_formatter_map::flag_formatter_map()
{
    using namespace walle::format_detail;
    flag_formatters.clear();
    flag_formatters.resize(256);
    flag_formatters[static_cast<size_t>('w')] = flag_formatter_ptr(new w_formatter());
    flag_formatters[static_cast<size_t>('W')] = flag_formatter_ptr(new W_formatter());
    flag_formatters[static_cast<size_t>('b')] = flag_formatter_ptr(new b_formatter());
    flag_formatters[static_cast<size_t>('B')] = flag_formatter_ptr(new B_formatter());
    flag_formatters[static_cast<size_t>('d')] = flag_formatter_ptr(new d_formatter());
    flag_formatters[static_cast<size_t>('e')] = flag_formatter_ptr(new e_formatter());
    flag_formatters[static_cast<size_t>('f')] = flag_formatter_ptr(new f_formatter());
    flag_formatters[static_cast<size_t>('m')] = flag_formatter_ptr(new m_formatter());
    flag_formatters[static_cast<size_t>('n')] = flag_formatter_ptr(new n_formatter());
    flag_formatters[static_cast<size_t>('o')] = flag_formatter_ptr(new o_formatter());
    flag_formatters[static_cast<size_t>('y')] = flag_formatter_ptr(new y_formatter());
    flag_formatters[static_cast<size_t>('Y')] = flag_formatter_ptr(new Y_formatter());
    flag_formatters[static_cast<size_t>('H')] = flag_formatter_ptr(new H_formatter());
    flag_formatters[static_cast<size_t>('h')] = flag_formatter_ptr(new h_formatter());
    flag_formatters[static_cast<size_t>('a')] = flag_formatter_ptr(new a_formatter());
    flag_formatters[static_cast<size_t>('A')] = flag_formatter_ptr(new A_formatter());
    flag_formatters[static_cast<size_t>('M')] = flag_formatter_ptr(new M_formatter());
    flag_formatters[static_cast<size_t>('S')] = flag_formatter_ptr(new S_formatter());
    flag_formatters[static_cast<size_t>('s')] = flag_formatter_ptr(new s_formatter());
    flag_formatters[static_cast<size_t>('i')] = flag_formatter_ptr(new i_formatter());
    flag_formatters[static_cast<size_t>('c')] = flag_formatter_ptr(new c_formatter());
    flag_formatters[static_cast<size_t>('F')] = flag_formatter_ptr(new F_formatter());
    flag_formatters[static_cast<size_t>('z')] = flag_formatter_ptr(new z_formatter());
    flag_formatters[static_cast<size_t>('Z')] = flag_formatter_ptr(new Z_formatter());
    flag_formatters[static_cast<size_t>('0')] = flag_formatter_ptr(new http_formatter());
    flag_formatters[static_cast<size_t>('1')] = flag_formatter_ptr(new iso8601_formatter());
    flag_formatters[static_cast<size_t>('2')] = flag_formatter_ptr(new iso8601_frac_formatter());
    flag_formatters[static_cast<size_t>('3')] = flag_formatter_ptr(new rfc822_formatter());
    flag_formatters[static_cast<size_t>('4')] = flag_formatter_ptr(new rfc1123_formatter());
    flag_formatters[static_cast<size_t>('5')] = flag_formatter_ptr(new rfc850_formatter());
    flag_formatters[static_cast<size_t>('6')] = flag_formatter_ptr(new rfc1036_formatter());
    flag_formatters[static_cast<size_t>('7')] = flag_formatter_ptr(new asctime_formatter());
    flag_formatters[static_cast<size_t>('8')] = flag_formatter_ptr(new sort_able_formatter());
}

flag_formatter_map::~flag_formatter_map()
{

}

flag_formatter_ptr flag_formatter_map::get_flag_formatter(char c)
{
    
    switch (c) {
        case 'w': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'W': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'b': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'B': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'd': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'e': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'f': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'm': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'n': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'o': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'y': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'Y': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'H': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'h': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'a': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'A': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'M': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'S': 
            return flag_formatters[static_cast<size_t>(c)];
        case 's': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'i': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'c': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'F': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'z': 
            return flag_formatters[static_cast<size_t>(c)];
        case 'Z': 
            return flag_formatters[static_cast<size_t>(c)];
        case '0': 
            return flag_formatters[static_cast<size_t>(c)];
        case '1': 
            return flag_formatters[static_cast<size_t>(c)];
        case '2': 
            return flag_formatters[static_cast<size_t>(c)];
        case '3': 
            return flag_formatters[static_cast<size_t>(c)];
        case '4': 
            return flag_formatters[static_cast<size_t>(c)];
        case '5': 
            return flag_formatters[static_cast<size_t>(c)];
        case '6': 
            return flag_formatters[static_cast<size_t>(c)];
        case '7': 
            return flag_formatters[static_cast<size_t>(c)];
        case '8': 
            return flag_formatters[static_cast<size_t>(c)];
        default:
            return flag_formatter_ptr();
    }
    return flag_formatter_ptr();
}

flag_formatter_ptr flag_formatter_map::get_char_formatter(char p)
{
    return flag_formatter_ptr(new walle::format_detail::p_formatter(p));
}
flag_formatter_ptr flag_formatter_map::get_string_formatter(const std::string &str)
{
    return flag_formatter_ptr(new walle::format_detail::P_formatter(str));
}

TimeFormatter::TimeFormatter()
{

}

TimeFormatter::~TimeFormatter()
{

}

bool TimeFormatter::set_property(const std::string& name, const std::string& value)
{
    if(name != "format") {
        return false;
    }
    _pattern = value;
    return compile_pattern(_pattern);
}

bool TimeFormatter::get_property(const std::string& name, std::string& value)
{
    if(name == "format") {
        value = _pattern;
        return true;
    }
    return false;
}

void TimeFormatter::format(walle::MemoryWriter &out, DateTime &dt)
{
    for(size_t i = 0; i < _formatters.size(); i++) {
        _formatters[i]->format(out, dt);
    }
}

/*
const std::string DateTimeFormat::ISO8601_FORMAT("%Y-%m-%dT%H:%M:%S%z");
const std::string DateTimeFormat::ISO8601_FRAC_FORMAT("%Y-%m-%dT%H:%M:%s%z");
const std::string DateTimeFormat::RFC822_FORMAT("%w, %e %b %y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC1123_FORMAT("%w, %e %b %Y %H:%M:%S %Z");
const std::string DateTimeFormat::HTTP_FORMAT("%w, %d %b %Y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC850_FORMAT("%W, %e-%b-%y %H:%M:%S %Z");
const std::string DateTimeFormat::RFC1036_FORMAT("%W, %e %b %y %H:%M:%S %Z");
const std::string DateTimeFormat::ASCTIME_FORMAT("%w %b %f %H:%M:%S %Y");
const std::string DateTimeFormat::SORTABLE_FORMAT("%Y-%m-%d %H:%M:%S");
*/

bool TimeFormatter::compile_pattern(std::string &pattern)
{   
    if (pattern == DateTimeFormat::HTTP_FORMAT || pattern == "$0") {
        handle_char('0');
        return true;
    } else if (pattern == DateTimeFormat::ISO8601_FORMAT || pattern == "$1") {
        handle_char('1');
        return true;
    } else if (pattern == DateTimeFormat::ISO8601_FRAC_FORMAT || pattern == "$2") {
        handle_char('2');
        return true;
    } else if (pattern == DateTimeFormat::RFC822_FORMAT || pattern == "$3") {
        handle_char('3');
        return true;
    } else if (pattern == DateTimeFormat::RFC1123_FORMAT || pattern == "$4") {
        handle_char('4');
        return true;
    } else if (pattern == DateTimeFormat::RFC850_FORMAT || pattern == "$5") {
        handle_char('5');
        return true;
    } else if (pattern == DateTimeFormat::RFC1036_FORMAT || pattern == "$6") {
        handle_char('6');
        return true;
    } else if (pattern == DateTimeFormat::ASCTIME_FORMAT || pattern == "$7") {
        handle_char('7');
        return true;
    } else if (pattern == DateTimeFormat::SORTABLE_FORMAT || pattern == "$8") {
        handle_char('8');
        return true;
    } 
    _formatters.clear();
    std::string::const_iterator  end = pattern.end();
    std::string::const_iterator  it = pattern.begin();
    std::string user_chars;
    for( ; it != end; ++it) {
        if(*it == '%') {
            if(user_chars.size() > 0) {
                flag_formatter_ptr ptr = 
                    flag_formatter_map::get_instance()->get_string_formatter(user_chars);
                _formatters.push_back(ptr);
                user_chars.clear();
            }
            if(++it != end) {
                if(*it == '%') {
                    user_chars.append(1, *it);
                    continue;
                }
                bool ret = handle_char(*it);
                if(!ret) {
                     _pattern.clear();
                     _formatters.clear();
                    return false;
                }
            } else {
                break;
            }
        } else {
            user_chars.append(1, *it);
        }
    }
    std::cout<<std::endl;
    if(user_chars.size() > 0) {
        flag_formatter_ptr ptr = 
                    flag_formatter_map::get_instance()->get_string_formatter(user_chars);
        _formatters.push_back(ptr);
        user_chars.clear();
    }
    return true;
}

bool TimeFormatter::handle_char(char c)
{
    flag_formatter_map *pmap = flag_formatter_map::get_instance();
    switch (c) {
         case 'w': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'W': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'b': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'B': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'd': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'e': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'f': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'm': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'n': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'o': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'y': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'Y': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'H': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'h': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'a': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'A': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'M': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'S': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 's': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'i': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'c': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'F': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'z': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'Z': 
            _formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case '0':
             _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        case '1':
             _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        case '2':
             _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        case '3':
            _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        case '4':
             _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        case '5':
             _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        case '6':
            _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        case '7':
             _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        case '8':
             _formatters.push_back(pmap->get_flag_formatter(c));
             break;
        default:
            return false; 
    }
    return true;
}

void TimeFormatter::set_user_formatter(const std::string &pattern, 
        const std::vector<flag_formatter_ptr> &fmt)
{
    WALLE_ASSERT(pattern.size() >0, "pattern empty");
    WALLE_ASSERT(fmt.size() >0, "fmt empty");
    _pattern = pattern;
    _formatters = fmt;
}
} //namespace walle