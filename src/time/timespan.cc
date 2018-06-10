#include <walle/time/timespan.h>
#include <algorithm>

namespace walle {

const Timespan::TimeDiff Timespan::kMilliSeconds = 1000;
const Timespan::TimeDiff Timespan::kSeconds      = 1000*Timespan::kMilliSeconds;
const Timespan::TimeDiff Timespan::kMinutes      = 60*Timespan::kSeconds;
const Timespan::TimeDiff Timespan::kHours        = 60*Timespan::kMinutes;
const Timespan::TimeDiff Timespan::kDays         = 24*Timespan::kHours;

Timespan::Timespan():
	_span(0)
{
}

	
Timespan::Timespan(TimeDiff microSeconds):
	_span(microSeconds)
{
}

Timespan::Timespan(long otherSeconds, long otherMicroSeconds)
    : _span(TimeDiff(otherSeconds)*kSeconds + otherMicroSeconds)
{
}

	
Timespan::Timespan(int otherDays, int otherHours, int otherMinutes, int otherSeconds, int otherMicroSeconds):
	_span(TimeDiff(otherMicroSeconds) + TimeDiff(otherSeconds)*kSeconds + TimeDiff(otherMinutes)*kMinutes + TimeDiff(otherHours)*kHours + TimeDiff(otherDays)*kDays)
{
}


Timespan::Timespan(const Timespan& timespan):
	_span(timespan._span)
{
}


Timespan::~Timespan()
{
}


Timespan& Timespan::operator = (const Timespan& timespan)
{
	_span = timespan._span;
	return *this;
}


Timespan& Timespan::operator = (TimeDiff microSeconds)
{
	_span = microSeconds;
	return *this;
}


Timespan& Timespan::assign(int otherDays, int otherHours, int otherMinutes, int otherSeconds, int otherMicroSeconds)
{
	_span = TimeDiff(otherMicroSeconds) + 
			TimeDiff(otherSeconds)*kSeconds + 
			TimeDiff(otherMinutes)*kMinutes + 
			TimeDiff(otherHours)*kHours + 
			TimeDiff(otherDays)*kDays;
	return *this;
}


Timespan& Timespan::assign(long otherSeconds, long otherMicroSeconds)
{
	_span = TimeDiff(otherSeconds)*kSeconds + TimeDiff(otherMicroSeconds);
	return *this;
}


void Timespan::swap(Timespan& timespan)
{
	std::swap(_span, timespan._span);
}


Timespan Timespan::operator + (const Timespan& d) const
{
	return Timespan(_span + d._span);
}   

Timespan Timespan::operator - (const Timespan& d) const
{
	return Timespan(_span - d._span);
}


Timespan& Timespan::operator += (const Timespan& d)
{
	_span += d._span;
	return *this;
}


Timespan& Timespan::operator -= (const Timespan& d)
{
	_span -= d._span;
	return *this;
}


Timespan Timespan::operator + (TimeDiff microSeconds) const
{
	return Timespan(_span + microSeconds);
}


Timespan Timespan::operator - (TimeDiff microSeconds) const
{
	return Timespan(_span - microSeconds);
}


Timespan& Timespan::operator += (TimeDiff microSeconds)
{
	_span += microSeconds;
	return *this;
}


Timespan& Timespan::operator -= (TimeDiff microSeconds)
{
	_span -= microSeconds;
	return *this;
}

} //namespace walle