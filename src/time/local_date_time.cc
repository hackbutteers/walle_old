#include <walle/time/local_date_time.h>
#include <walle/time/time_zone.h>
namespace walle {

LocalDateTime::LocalDateTime()
{
	determine_time_zone_differential(true);
}

LocalDateTime::LocalDateTime(int otherYear, int otherMonth, int otherDay, int otherHour, int otherMinute, int otherSecond, int otherMillisecond, int otherMicrosecond):
	_dateTime(otherYear, otherMonth, otherDay, otherHour, otherMinute, otherSecond, otherMillisecond, otherMicrosecond)
{
	determine_time_zone_differential();
}

LocalDateTime::LocalDateTime(int otherTzd, int otherYear, int otherMonth, int otherDay, int otherHour, int otherMinute, int otherSecond, int otherMillisecond, int otherMicrosecond):
	_dateTime(otherYear, otherMonth, otherDay, otherHour, otherMinute, otherSecond, otherMillisecond, otherMicrosecond),
	_tzd(otherTzd)
{
}

LocalDateTime::LocalDateTime(double otherJulianDay):
	_dateTime(otherJulianDay)
{
	determine_time_zone_differential(true);
}



LocalDateTime::LocalDateTime(int otherTzd, double otherJulianDay):
	_dateTime(otherJulianDay),
	_tzd(otherTzd)
{
	adjust_for_time_zone_differential();
}

LocalDateTime::LocalDateTime(const DateTime& dateTime):
	_dateTime(dateTime)
{
	determine_time_zone_differential(true);
}

LocalDateTime::LocalDateTime(int otherTzd, const DateTime& otherDateTime):
	_dateTime(otherDateTime),
	_tzd(otherTzd)
{
    adjust_for_time_zone_differential();
}

LocalDateTime::LocalDateTime(int otherTzd, const DateTime& otherDateTime, bool adjust):
	_dateTime(otherDateTime),
	_tzd(otherTzd)
{
	if (adjust) {
        adjust_for_time_zone_differential();
    }
}

LocalDateTime::LocalDateTime(const LocalDateTime& dateTime):
	_dateTime(dateTime._dateTime),
	_tzd(dateTime._tzd)
{
}

LocalDateTime::LocalDateTime(Timestamp::UtcTimeVal utcTimeVal, Timestamp::TimeDiff diff, int otherTzd):
	_dateTime(utcTimeVal, diff),
	_tzd(otherTzd)
{
	 adjust_for_time_zone_differential();
}

LocalDateTime::~LocalDateTime()
{
}

LocalDateTime& LocalDateTime::operator = (const LocalDateTime& dateTime)
{
	if (&dateTime != this) {
		_dateTime = dateTime._dateTime;
		_tzd      = dateTime._tzd;
	}
	return *this;
}

LocalDateTime& LocalDateTime::operator = (const Timestamp& otherTimestamp)
{
	if (otherTimestamp != timestamp()) {
		_dateTime = otherTimestamp;
		determine_time_zone_differential(true);
	}
	return *this;
}

LocalDateTime& LocalDateTime::operator = (double otherJulianDay)
{
	_dateTime = otherJulianDay;
	determine_time_zone_differential(true);
	return *this;
}

LocalDateTime& LocalDateTime::assign(int otherYear, int otherMonth, int otherDay, int otherHour, int otherMinute, int otherSecond, int otherMillisecond, int otherMicroseconds)
{
	_dateTime.assign(otherYear, otherMonth, otherDay, otherHour, otherMinute, otherSecond, otherMillisecond, otherMicroseconds);
	determine_time_zone_differential(false);
	return *this;
}

LocalDateTime& LocalDateTime::assign(int otherTzd, int otherYear, int otherMonth, int otherDay, int otherHour, int otherMinute, int otherSecond, int otherMillisecond, int otherMicroseconds)
{
	_dateTime.assign(otherYear, otherMonth, otherDay, otherHour, otherMinute, otherSecond, otherMillisecond, otherMicroseconds);
	_tzd = otherTzd;
	return *this;
}

LocalDateTime& LocalDateTime::assign(int otherTzd, double otherJulianDay)
{
	_tzd      = otherTzd;
	_dateTime = otherJulianDay;
	adjust_for_time_zone_differential();
	return *this;
}

void LocalDateTime::swap(LocalDateTime& dateTime)
{
	_dateTime.swap(dateTime._dateTime);
	std::swap(_tzd, dateTime._tzd);
}

DateTime LocalDateTime::utc() const
{
	return DateTime(_dateTime.utc_time(), -((Timestamp::TimeDiff) _tzd)*Timespan::kSeconds);
}

bool LocalDateTime::operator == (const LocalDateTime& dateTime) const
{
	return utc_time() == dateTime.utc_time();
}


bool LocalDateTime::operator != (const LocalDateTime& dateTime) const	
{
	return utc_time() != dateTime.utc_time();
}


bool LocalDateTime::operator <  (const LocalDateTime& dateTime) const	
{
	return utc_time() < dateTime.utc_time();
}


bool LocalDateTime::operator <= (const LocalDateTime& dateTime) const	
{
	return utc_time() <= dateTime.utc_time();
}

bool LocalDateTime::operator >  (const LocalDateTime& dateTime) const	
{
	return utc_time() > dateTime.utc_time();
}


bool LocalDateTime::operator >= (const LocalDateTime& dateTime) const	
{
	return utc_time() >= dateTime.utc_time();
}

LocalDateTime LocalDateTime::operator + (const Timespan& span) const
{
	// First calculate the adjusted UTC time, then calculate the
	// locally adjusted time by constructing a new LocalDateTime.
	DateTime tmp(utc_time(), span.total_microseconds());
	return LocalDateTime(tmp);
}


LocalDateTime LocalDateTime::operator - (const Timespan& span) const
{
	// First calculate the adjusted UTC time, then calculate the
	// locally adjusted time by constructing a new LocalDateTime.
	DateTime tmp(utc_time(), -span.total_microseconds());
	return LocalDateTime(tmp);
}


Timespan LocalDateTime::operator - (const LocalDateTime& dateTime) const
{
	return Timespan((utc_time() - dateTime.utc_time())/10);
}

LocalDateTime& LocalDateTime::operator += (const Timespan& span)
{
	// Use the same trick as in operator+. Create a UTC time, adjust
	// it for the span, and convert back to LocalDateTime. This will
	// recalculate the tzd correctly in the case where the addition
	// crosses a DST boundary.
	*this = DateTime(utc_time(), span.total_microseconds());
	return *this;
}


LocalDateTime& LocalDateTime::operator -= (const Timespan& span)
{
	// Use the same trick as in operator-. Create a UTC time, adjust
	// it for the span, and convert back to LocalDateTime. This will
	// recalculate the tzd correctly in the case where the subtraction
	// crosses a DST boundary.
	*this = DateTime(utc_time(), -span.total_microseconds());
	return *this;
}


void LocalDateTime::determine_time_zone_differential(bool adjust)
{
    if (adjust) {
	    std::time_t epochTime = _dateTime.timestamp().second();
		std::tm broken;
		localtime_r(&epochTime, &broken);
		_tzd = (TimeZone::utc_offset() + ((broken.tm_isdst == 1) ? 3600 : 0));
		adjust_for_time_zone_differential();
	} else {
		int dst;
		day_light_saveing_time_offset(dst);
		_tzd = (TimeZone::utc_offset() + dst);
	}

}


std::time_t LocalDateTime::day_light_saveing_time_offset(int& rDstOffset) const
{
	std::time_t local;
	std::tm     broken;

	broken.tm_year  = (_dateTime.year() - 1900);
	broken.tm_mon   = (_dateTime.month() - 1);
	broken.tm_mday  = _dateTime.day();
	broken.tm_hour  = _dateTime.hour();
	broken.tm_min   = _dateTime.minute();
	broken.tm_sec   = _dateTime.second();
	broken.tm_isdst = -1;
	local = std::mktime(&broken);
	
	rDstOffset = (broken.tm_isdst == 1) ? 3600 : 0;
	return local;
}

} // namespace walle