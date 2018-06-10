#ifndef WALLE_TIME_TIMESPAN_H_
#define WALLE_TIME_TIMESPAN_H_
#include <walle/time/timestamp.h>
#include <cstdint>

namespace walle {

class Timespan {
public:

    typedef Timestamp::TimeDiff TimeDiff;
    
    /** 
     * @brief  default ctor for zero
     * @note   
     * @retval 
     */
    Timespan();

    /** 
     * @brief Creates a Timespan with given time.
     * @note given time should be micro seconds
     * @param  microseconds: 
     * @retval 
     */
    Timespan(TimeDiff microseconds);

    /** 
     * @brief Creates a Timespan with given time.
     * @note   
     * @param  seconds: 
     * @param  microseconds: 
     * @retval 
     */
    Timespan(long seconds, long microseconds);

    /** 
     * @brief Creates a Timespan with given time.
     * @note   
     * @param  days: 
     * @param  hours: 
     * @param  minutes: 
     * @param  seconds: 
     * @param  microSeconds: 
     * @retval 
     */
    Timespan(int days, int hours, int minutes, int seconds, int microSeconds);

    /** 
     * @brief Creates a Timespan with other one. 
     * @note   
     * @param  timespan: 
     * @retval 
     */
    Timespan(const Timespan& timespan);

    /** 
     * @brief  
     * @note   
     * @retval 
     */
    ~Timespan();
    
    /** 
     * @brief  
     * @note   
     * @param  timespan: 
     * @retval None
     */
    Timespan& operator = (const Timespan& timespan);

    /** 
     * @brief 
     * @note   
     * @param  microseconds: 
     * @retval None
     */
    Timespan& operator = (TimeDiff microseconds);

    /** 
     * @brief  
     * @note   
     * @param  days: 
     * @param  hours: 
     * @param  minutes: 
     * @param  seconds: 
     * @param  microSeconds: 
     * @retval 
     */
    Timespan& assign(int days, int hours, int minutes, int seconds, int microSeconds);

    /** 
     * @brief  
     * @note   
     * @param  seconds: 
     * @param  microseconds: 
     * @retval 
     */
    Timespan& assign(long seconds, long microseconds);

    /** 
     * @brief Swaps the Timespan with another one. 
     * @note   
     * @param  timespan: 
     * @retval None
     */
    void swap(Timespan& timespan);

    bool operator == (const Timespan& ts) const;
	bool operator != (const Timespan& ts) const;
	bool operator >  (const Timespan& ts) const;
	bool operator >= (const Timespan& ts) const;
	bool operator <  (const Timespan& ts) const;
	bool operator <= (const Timespan& ts) const;

	bool operator == (TimeDiff microSeconds) const;
	bool operator != (TimeDiff microSeconds) const;
	bool operator >  (TimeDiff microSeconds) const;
	bool operator >= (TimeDiff microSeconds) const;
	bool operator <  (TimeDiff microSeconds) const;
    bool operator <= (TimeDiff microSeconds) const;
    
    Timespan operator + (const Timespan& d) const;
	Timespan operator - (const Timespan& d) const;
	Timespan& operator += (const Timespan& d);
	Timespan& operator -= (const Timespan& d);

	Timespan operator + (TimeDiff microSeconds) const;
	Timespan operator - (TimeDiff microSeconds) const;
	Timespan& operator += (TimeDiff microSeconds);
	Timespan& operator -= (TimeDiff microSeconds);

    /** 
     * @brief  return the number of days.
     * @note   
     * @retval 
     */
    int days() const;

    /** 
     * @brief  return the number of hours (0 to 23).
     * @note   
     * @retval 
     */
    int hours() const;

    /** 
     * @brief  return the total number of hours.
     * @note   
     * @retval 
     */
    int total_hours() const;

    /** 
     * @brief  return the number of minutes (0 to 59).
     * @note   
     * @retval 
     */
    int minutes() const;

    /** 
     * @brief  return the total number of minutes.
     * @note   
     * @retval 
     */
    int total_minutes() const;

    /** 
     * @brief  return the number of seconds (0 to 59).
     * @note   
     * @retval 
     */
    int seconds() const;

    /** 
     * @brief  return the total number of seconds.
     * @note   
     * @retval 
     */
    int total_seconds() const;

    /** 
     * @brief  return the number of milliseconds (0 to 999999).
     * @note   
     * @retval 
     */
    int milliseconds() const;

    /** 
     * @brief  return the total number of milliseconds.
     * @note   
     * @retval 
     */
    TimeDiff total_milliseconds() const;

    /** 
     * @brief  return the fractions of a millisecond in microseconds (0 to 999).
     * @note   
     * @retval 
     */
    int microseconds_mod_milliseconds() const;

    /** 
     * @brief  return  the fractions of a second in microseconds (0 to 999999).
     * @note   
     * @retval 
     */
    TimeDiff microseconds() const;

    /** 
     * @brief  convert to struct timespec
     * @note   
     * @retval None
     */
    operator timespec() const;

    /** 
     * @brief  return  the total number of microseconds.
     * @note   
     * @retval 
     */
    TimeDiff total_microseconds() const;

    static const TimeDiff kMilliSeconds;  //!< The number of microseconds in a millisecond.
	static const TimeDiff kSeconds;        //!< The number of microseconds in a second.
	static const TimeDiff kMinutes;       //!< The number of microseconds in a minute.
	static const TimeDiff kHours;         //!< The number of microseconds in a hour.
	static const TimeDiff kDays;          //!< The number of microseconds in a day.   
private:
    int64_t    _span;    
};

WALLE_FORCE_INLINE int Timespan::days() const
{
	return int(_span/kDays);
}


WALLE_FORCE_INLINE int Timespan::hours() const
{
	return int((_span/kHours) % 24);
}

	
WALLE_FORCE_INLINE int Timespan::total_hours() const
{
	return int(_span/kHours);
}

	
WALLE_FORCE_INLINE int Timespan::minutes() const
{
	return int((_span/kMinutes) % 60);
}

	
WALLE_FORCE_INLINE int Timespan::total_minutes() const
{
	return int(_span/kMinutes);
}

	
WALLE_FORCE_INLINE int Timespan::seconds() const
{
	return int((_span/kSeconds) % 60);
}

	
WALLE_FORCE_INLINE int Timespan::total_seconds() const
{
	return int(_span/kSeconds);
}

	
WALLE_FORCE_INLINE int Timespan::milliseconds() const
{
	return int((_span/kMilliSeconds) % 1000);
}

	
WALLE_FORCE_INLINE Timespan::TimeDiff Timespan::total_milliseconds() const
{
	return _span/kMilliSeconds;
}

	
WALLE_FORCE_INLINE int Timespan::microseconds_mod_milliseconds() const
{
	return int(_span % 1000);
}


WALLE_FORCE_INLINE Timespan::TimeDiff Timespan::microseconds() const
{
	return int(_span % 1000000);
}

	
WALLE_FORCE_INLINE Timespan::TimeDiff Timespan::total_microseconds() const
{
	return _span;
}


WALLE_FORCE_INLINE bool Timespan::operator == (const Timespan& ts) const
{
	return _span == ts._span;
}


WALLE_FORCE_INLINE bool Timespan::operator != (const Timespan& ts) const
{
	return _span != ts._span;
}


WALLE_FORCE_INLINE bool Timespan::operator >  (const Timespan& ts) const
{
	return _span > ts._span;
}


WALLE_FORCE_INLINE bool Timespan::operator >= (const Timespan& ts) const
{
	return _span >= ts._span;
}


WALLE_FORCE_INLINE bool Timespan::operator <  (const Timespan& ts) const
{
	return _span < ts._span;
}


WALLE_FORCE_INLINE bool Timespan::operator <= (const Timespan& ts) const
{
	return _span <= ts._span;
}


WALLE_FORCE_INLINE bool Timespan::operator == (TimeDiff microSeconds) const
{
	return _span == microSeconds;
}


WALLE_FORCE_INLINE bool Timespan::operator != (TimeDiff microSeconds) const
{
	return _span != microSeconds;
}


WALLE_FORCE_INLINE bool Timespan::operator >  (TimeDiff microSeconds) const
{
	return _span > microSeconds;
}


WALLE_FORCE_INLINE bool Timespan::operator >= (TimeDiff microSeconds) const
{
	return _span >= microSeconds;
}


WALLE_FORCE_INLINE bool Timespan::operator <  (TimeDiff microSeconds) const
{
	return _span < microSeconds;
}


WALLE_FORCE_INLINE bool Timespan::operator <= (TimeDiff microSeconds) const
{
	return _span <= microSeconds;
}


WALLE_FORCE_INLINE void swap(Timespan& s1, Timespan& s2)
{
	s1.swap(s2);
}

WALLE_FORCE_INLINE Timespan::operator timespec() const
{
    struct timespec ts;
    ts.tv_sec = _span / kSeconds;
    ts.tv_nsec = (_span % kSeconds);
    return ts;
}
} //namespace walle
#endif //WALLE_TIME_TIMESPAN_H_