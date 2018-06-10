#include <walle/time/timestamp.h>
#include <walle/time/timespan.h>
#include <algorithm>
#undef min
#undef max
#include <limits>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>

namespace walle {

const Timestamp::TimeVal Timestamp::kTimeValMin = std::numeric_limits<Timestamp::TimeVal>::min();
const Timestamp::TimeVal Timestamp::kTimeValMax = std::numeric_limits<Timestamp::TimeVal>::max();

Timestamp::Timestamp()
{
	update();
}


Timestamp::Timestamp(TimeVal tv)
{
	_ts = tv;
}

Timestamp::Timestamp(const Timestamp& other)
{
	_ts = other._ts;
}


Timestamp::~Timestamp()
{
}


Timestamp& Timestamp::operator = (const Timestamp& other)
{
	_ts = other._ts;
	return *this;
}

Timestamp& Timestamp::operator = (TimeVal tv)
{
	_ts = tv;
	return *this;
}


void Timestamp::swap(Timestamp& timestamp)
{
	std::swap(_ts, timestamp._ts);
}

Timestamp Timestamp::from_second(std::time_t t)
{
	return Timestamp(TimeVal(t)*ratio());
}

Timestamp Timestamp::from_micro_second(Timestamp::TimeVal val)
{
	return Timestamp(TimeVal(val)*ratio());
}

Timestamp Timestamp::from_utc_time(UtcTimeVal val)
{
	val -= (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
	val /= 10;
	return Timestamp(val);
}

Timestamp Timestamp::max()
{
	return Timestamp(kTimeValMax);
}

Timestamp Timestamp::min()
{
	return Timestamp(0);
}

void Timestamp::update()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL)) {
		_ts = 0;
		return ;
	}

	_ts = TimeVal(tv.tv_sec)*ratio() + tv.tv_usec;	
}

Timestamp  Timestamp::operator +  (const Timespan& span) const
{
	return *this + span.total_microseconds();
}


Timestamp  Timestamp::operator -  (const Timespan& span) const
{
	return *this - span.total_microseconds();
}


Timestamp& Timestamp::operator += (const Timespan& span)
{
	return *this += span.total_microseconds();
}


Timestamp& Timestamp::operator -= (const Timespan& span)
{
	return *this -= span.total_microseconds();
}

WALLE_FORCE_INLINE bool Timestamp::is_elapsed(const Timespan &span) const
{
    Timestamp now;
    return (now - *this) >= span.microseconds();
}

std::tm Timestamp::local_time() const
{
	std::tm t;
	std::time_t time = second();
	localtime_r(&time, &t);
	return t;
}

std::tm Timestamp::gmtime() const
{
	std::tm t;
	std::time_t time = second();
	gmtime_r(&time, &t);
	return t;	
}
} //namespace walle