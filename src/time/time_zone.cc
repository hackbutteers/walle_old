#include <walle/time/time_zone.h>
#include <walle/sys/mutex.h>
#include <ctime>

namespace walle {
class TZInfo
{
public:
	TZInfo()
	{
		tzset();
	}
	
	int time_zone()
	{
		UniqueLock<Mutex> lock(_mutex);
		tzset();
		return -timezone;
	}
	
	const char* name(bool dst)
	{
		UniqueLock<Mutex> lock(_mutex);
		tzset();		
		return tzname[dst ? 1 : 0];
	}
		
private:
	Mutex _mutex;
};

TZInfo& get_tzinfo()
{
	static TZInfo tzinfo;
	return tzinfo;
}
int TimeZone::utc_offset()
{
	return get_tzinfo().time_zone();
}

int TimeZone::day_light_saving_time()
{
	Timestamp timestamp;
	std::time_t now = timestamp.second();
	struct std::tm t;
	localtime_r(&now, &t);		
	return t.tm_isdst == 1 ? 3600 : 0;
}

bool TimeZone::is_day_light_saving_time(const Timestamp& timestamp)
{
	std::time_t now = timestamp.second();
	struct std::tm t;
	localtime_r(&now, &t);
	return t.tm_isdst > 0;
}

std::string TimeZone::name()
{
	return std::string(get_tzinfo().name(day_light_saving_time() != 0));
}

std::string TimeZone::standard_name()
{
	return std::string(get_tzinfo().name(false));
}

std::string TimeZone::day_light_saving_time_name()
{
	return std::string(get_tzinfo().name(true));
}

int TimeZone::time_zone_differential()
{
	return utc_offset() + day_light_saving_time();
}

LocalTimeZoneDiff& get_local_tzd()
{
    static LocalTimeZoneDiff ltzd;
    return ltzd;
}

} //namespace walle