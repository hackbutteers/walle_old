#include <walle/time/date_time.h>
#include <walle/time/timespan.h>
#include <algorithm>
#include <cmath>

namespace walle {

WALLE_FORCE_INLINE double DateTime::to_julianday(Timestamp::UtcTimeVal utcTime)
{
	double utcDays = double(utcTime)/864000000000.0;
	return utcDays + 2299160.5; // first day of Gregorian reform (Oct 15 1582)
}


WALLE_FORCE_INLINE Timestamp::UtcTimeVal DateTime::to_utc_time(double julianDay)
{
	return Timestamp::UtcTimeVal((julianDay - 2299160.5)*864000000000.0);
}


DateTime::DateTime(double otherJulianDay):
	_utc_time(to_utc_time(otherJulianDay))
{
	compute_gregorian(otherJulianDay);
}


DateTime::DateTime(Timestamp::UtcTimeVal otherUtcTime, Timestamp::TimeDiff diff):
	_utc_time(otherUtcTime + diff*10)
{
	compute_gregorian(julianday());
	compute_daytime();
}

DateTime::DateTime(int otherYear, int otherMonth, int otherDay, 
                    int otherHour, int otherMinute, int otherSecond, 
                    int otherMillisecond, int otherMicrosecond):
	_year(otherYear),
	_month(otherMonth),
	_day(otherDay),
	_hour(otherHour),
	_minute(otherMinute),
	_second(otherSecond),
	_millisecond(otherMillisecond),
	_microsecond(otherMicrosecond)
{
	WALLE_ASSERT (_year >= 0 && _year <= 9999, "");
	WALLE_ASSERT (_month >= 1 && _month <= 12, "");
	WALLE_ASSERT (_day >= 1 && _day <= days_of_month(_year, _month), "");
	WALLE_ASSERT (_hour >= 0 && _hour <= 23, "");
	WALLE_ASSERT (_minute >= 0 && _minute <= 59, "");
	WALLE_ASSERT (_second >= 0 && _second <= 59, "");
	WALLE_ASSERT (_millisecond >= 0 && _millisecond <= 999, "");
	WALLE_ASSERT (_microsecond >= 0 && _microsecond <= 999, "");
	
    _utc_time = to_utc_time(to_julianday(_year, _month, _day)) + 
            10*(_hour*Timespan::kHours + _minute*Timespan::kMinutes + 
            _second*Timespan::kSeconds + 
            _millisecond*Timespan::kMilliSeconds + _microsecond);
}

DateTime::DateTime(const Timestamp& rTimestamp):
	_utc_time(rTimestamp.utc_time())
{
	compute_gregorian(julianday());
	compute_daytime();
}

DateTime::DateTime()
{
	Timestamp now;
	_utc_time = now.utc_time();
	compute_gregorian(julianday());
	compute_daytime();
}

DateTime::~DateTime()
{
}


DateTime::DateTime(const DateTime& dateTime):
	_utc_time(dateTime._utc_time),
	_year(dateTime._year),
	_month(dateTime._month),
	_day(dateTime._day),
	_hour(dateTime._hour),
	_minute(dateTime._minute),
	_second(dateTime._second),
	_millisecond(dateTime._millisecond),
	_microsecond(dateTime._microsecond)
{
}

DateTime& DateTime::operator = (double otherJulianDay)
{
	_utc_time = to_utc_time(otherJulianDay);
	compute_gregorian(otherJulianDay);
	return *this;
}

DateTime& DateTime::operator = (const Timestamp& otherTimestamp)
{
	_utc_time = otherTimestamp.utc_time();
	compute_gregorian(julianday());
	compute_daytime();
	return *this;
}

DateTime& DateTime::operator = (const DateTime& dateTime)
{
	if (&dateTime != this) {
		_utc_time     = dateTime._utc_time;
		_year        = dateTime._year;
		_month       = dateTime._month;
		_day         = dateTime._day;
		_hour        = dateTime._hour;
		_minute      = dateTime._minute;
		_second      = dateTime._second;
		_millisecond = dateTime._millisecond;
		_microsecond = dateTime._microsecond;
	}
	return *this;
}

DateTime& DateTime::assign(int otherYear, int otherMonth, 
                        int otherDay, int otherHour, 
                        int otherMinute, int otherSecond, 
                        int otherMillisecond, int otherMicrosecond)
{
	WALLE_ASSERT (otherYear >= 0 && otherYear <= 9999, "");
	WALLE_ASSERT (otherMonth >= 1 && otherMonth <= 12, "");
	WALLE_ASSERT (otherDay >= 1 && otherDay <= days_of_month(otherYear, otherMonth), "");
	WALLE_ASSERT (otherHour >= 0 && otherHour <= 23, "");
	WALLE_ASSERT (otherMinute >= 0 && otherMinute <= 59, "");
	WALLE_ASSERT (otherSecond >= 0 && otherSecond <= 59, "");
	WALLE_ASSERT (otherMillisecond >= 0 && otherMillisecond <= 999, "");
	WALLE_ASSERT (otherMicrosecond >= 0 && otherMicrosecond <= 999, "");

    _utc_time     = to_utc_time(to_julianday(otherYear, otherMonth, otherDay)) + 
                                10*(otherHour*Timespan::kHours + 
                                otherMinute*Timespan::kMinutes + 
                                otherSecond*Timespan::kSeconds + 
                                otherMillisecond*Timespan::kMilliSeconds + otherMicrosecond);
	_year        = otherYear;
	_month       = otherMonth;
	_day         = otherDay;
	_hour        = otherHour;
	_minute      = otherMinute;
	_second      = otherSecond;
	_millisecond = otherMillisecond;
	_microsecond = otherMicrosecond;
	
	return *this;
}

void DateTime::swap(DateTime& dateTime)
{
	std::swap(_utc_time, dateTime._utc_time);
	std::swap(_year, dateTime._year);
	std::swap(_month, dateTime._month);
	std::swap(_day, dateTime._day);
	std::swap(_hour, dateTime._hour);
	std::swap(_minute, dateTime._minute);
	std::swap(_second, dateTime._second);
	std::swap(_millisecond, dateTime._millisecond);
	std::swap(_microsecond, dateTime._microsecond);
}

int DateTime::day_of_week() const
{
	return int((std::floor(julianday() + 1.5))) % 7;
}

int DateTime::days_of_month(int year, int month)
{
	WALLE_ASSERT(month >= 1 && month <= 12, "month overflow");

	static int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
	if (month == 2 && is_leap_year(year)) {
        return 29;
    } else {
		return daysOfMonthTable[month];
    }
    
}

int DateTime::day_of_year() const
{
	int doy = 0;
	for (int currentMonth = 1; currentMonth < _month; ++currentMonth)
		doy += days_of_month(_year, currentMonth);
	doy += _day;
	return doy;
}

bool DateTime::is_valid(int year, int month, int day, int hour, int minute, 
						int second, int millisecond, int microsecond)
{
	return
		(year >= 0 && year <= 9999) &&
		(month >= 1 && month <= 12) &&
		(day >= 1 && day <= days_of_month(year, month)) &&
		(hour >= 0 && hour <= 23) &&
		(minute >= 0 && minute <= 59) &&
		(second >= 0 && second <= 59) &&
		(millisecond >= 0 && millisecond <= 999) &&
		(microsecond >= 0 && microsecond <= 999);
}

int DateTime::week(WeekDays firstDayOfWeek) const
{
	int wd = static_cast<int>(firstDayOfWeek);
	WALLE_ASSERT(wd >= 0 && wd <= 6, "over flow");
	/// find the first firstDayOfWeek.
	int baseDay = 1;
	while (DateTime(_year, 1, baseDay).day_of_week() != wd) ++baseDay;

	int doy  = day_of_year();
	int offs = baseDay <= 4 ? 0 : 1; 
	if (doy < baseDay) {
		return offs;
	} else {
		return (doy - baseDay)/7 + 1 + offs;
	}
}


double DateTime::julianday() const
{
	return to_julianday(_utc_time);
}

DateTime DateTime::operator + (const Timespan& span) const
{
	return DateTime(_utc_time, span.total_microseconds());
}


DateTime DateTime::operator - (const Timespan& span) const
{
	return DateTime(_utc_time, -span.total_microseconds());
}


Timespan DateTime::operator - (const DateTime& dateTime) const
{
	return Timespan((_utc_time - dateTime._utc_time)/10);
}

DateTime& DateTime::operator += (const Timespan& span)
{
	_utc_time += span.total_microseconds()*10;
	compute_gregorian(julianday());
	compute_daytime();
	return *this;
}


DateTime& DateTime::operator -= (const Timespan& span)
{
	_utc_time -= span.total_microseconds()*10;
	compute_gregorian(julianday());
	compute_daytime();
	return *this;
}

void DateTime::make_utc(int tzd)
{
	operator -= (Timespan(((Timestamp::TimeDiff) tzd)*Timespan::kSeconds));
}

void DateTime::make_local(int tzd)
{
	operator += (Timespan(((Timestamp::TimeDiff) tzd)*Timespan::kSeconds));
}

double DateTime::to_julianday(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
{
	// lookup table for (153*month - 457)/5 - note that 3 <= month <= 14.
	static int lookup[] = {-91, -60, -30, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337};
 
	// day to double
	double dday = double(day) + ((double((hour*60 + minute)*60 + second)*1000 + millisecond)*1000 + microsecond)/86400000000.0;
	if (month < 3)
	{
		month += 12;
		--year;
	}
	double dyear = double(year);
	return dday + lookup[month] + 365*year + std::floor(dyear/4) - std::floor(dyear/100) + std::floor(dyear/400) + 1721118.5;
}

void DateTime::check_limit(short& lower, short& higher, short limit)
{
	if (lower >= limit) {
		higher += short(lower / limit);
		lower   = short(lower % limit);
	}
}

void DateTime::normalize()
{
	check_limit(_microsecond, _millisecond, 1000);
	check_limit(_millisecond, _second, 1000);
	check_limit(_second, _minute, 60);
	check_limit(_minute, _hour, 60);
	check_limit(_hour, _day, 24);

	if (_day > days_of_month(_year, _month)) {
		_day -= days_of_month(_year, _month);
		if (++_month > 12) {
			++_year;
			_month -= 12;
		}
	}
}

void DateTime::compute_gregorian(double otherJulianDay)
{
	double z    = std::floor(otherJulianDay - 1721118.5);
	double r    = otherJulianDay - 1721118.5 - z;
	double g    = z - 0.25;
	double a    = std::floor(g / 36524.25);
	double b    = a - std::floor(a/4);
	_year       = short(std::floor((b + g)/365.25));
	double c    = b + z - std::floor(365.25*_year);
	_month      = short(std::floor((5*c + 456)/153));
	double dday = c - std::floor((153.0*_month - 457)/5) + r;
	_day        = short(dday);
	if (_month > 12) {
		++_year;
		_month -= 12;
	}
	r      *= 24;
	_hour   = short(std::floor(r));
	r      -= std::floor(r);
	r      *= 60;
	_minute = short(std::floor(r));
	r      -= std::floor(r);
	r      *= 60;
	_second = short(std::floor(r));
	r      -= std::floor(r);
	r      *= 1000;
	_millisecond = short(std::floor(r));
	r      -= std::floor(r);
	r      *= 1000;
	_microsecond = short(r + 0.5);

	normalize();
	WALLE_ASSERT (_month >= 1 && _month <= 12, "");
	WALLE_ASSERT (_day >= 1 && _day <= days_of_month(_year, _month), "");
	WALLE_ASSERT (_hour >= 0 && _hour <= 23, "");
	WALLE_ASSERT (_minute >= 0 && _minute <= 59, "");
	WALLE_ASSERT (_second >= 0 && _second <= 59,"");
	WALLE_ASSERT (_millisecond >= 0 && _millisecond <= 999, "");
	WALLE_ASSERT (_microsecond >= 0 && _microsecond <= 999, "");
}


void DateTime::compute_daytime()
{
	Timespan span(_utc_time/10);
    int spanHour = span.hours();
	// Due to double rounding issues, the previous call to computeGregorian()
	// may have crossed into the next or previous day. We need to correct that.
	if (spanHour == 23 && _hour == 0) {
		_day--;
		if (_day == 0) {
			_month--;
			if (_month == 0) {
				_month = 12;
				_year--;
			}
			_day = days_of_month(_year, _month);
		}
	} else if (spanHour == 0 && _hour == 23) {
		_day++;
		if (_day > days_of_month(_year, _month)) {
			_month++;
			if (_month > 12) {
				_month = 1;
				_year++;
			}
			_day = 1;
		}
	}
	_hour        = spanHour;
	_minute      = span.minutes();
	_second      = span.seconds();
	_millisecond = span.milliseconds();
	_microsecond = span.microseconds();
}

} //namespace walle