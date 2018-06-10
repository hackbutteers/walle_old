#ifndef WALLE_TIME_TIMESTAMP_H_
#define WALLE_TIME_TIMESTAMP_H_
#include <walle/config/base.h>
#include <type_traits>
#include <cstdint>
#include <ctime>
namespace walle {
// forward declare Timespan
class Timespan;



/**
 * @brief A Timestamp stores a monotonic* time value
 * with (theoretical) microseconds resolution.
 * Timestamps can be compared with each other
 * and simple arithmetic is supported.
 *
 * [*] Note that Timestamp values are only monotonic as
 * long as the systems's clock is monotonic as well
 * (and not, e.g. set back due to time synchronization
 * or other reasons).
 *
 * Timestamps are UTC (Coordinated Universal Time)
 * based and thus independent of the timezone
 * in effect on the system.
 *
 * The internal reference time is the Unix epoch, 
 * midnight, January 1, 1970.
 * 
 */
class Timestamp{
public:
    /**
     * Monotonic UTC time value in microsecond resolution,
     * with base time midnight, January 1, 1970.
     */
    typedef int64_t TimeVal;

    /*!
    * Monotonic UTC time value in 100 nanosecond resolution,
    * with base time midnight, October 15, 1582.
    */
    typedef int64_t UtcTimeVal;

    /*!
    * Difference between two TimeVal values in microseconds.
    */	
    typedef int64_t TimeDiff;

    static const TimeVal kTimeValMin;
    static const TimeVal kTimeValMax;
public:
    /** 
     * @brief Creates a timestamp with the current time.
     * @note   
     * @retval 
     */
    Timestamp();

    /** 
     * @brief Creates a timestamp from the given time value
     * (microseconds since midnight, January 1, 1970).
     * @note   
     * @param  tv: microsecond to make a stamp 
     * @retval 
     */
    Timestamp(TimeVal tv);

    /** 
     * @brief  copy constructor
     * @note   
     * @param  &rhs: 
     * @retval 
     */
    Timestamp(const Timestamp &rhs);

    /** 
     * @brief  
     * @note   
     * @retval 
     */
    ~Timestamp();
    /** 
     * @brief  assign operator
     * @note   
     * @param  other: 
     * @retval None
     */
    Timestamp& operator = (const Timestamp& other);
    
    Timestamp& operator = (TimeVal tv);
    
    /** 
     * @brief  swap two stamp
     * @note   
     * @param  &rhs: 
     * @retval None
     */
    void swap(Timestamp &rhs);

    /** 
     * @brief Updates the Timestamp with the current time. 
     * @note   
     * @retval None
     */
    void update();

    bool operator == (const Timestamp& ts) const;
	bool operator != (const Timestamp& ts) const;
	bool operator >  (const Timestamp& ts) const;
	bool operator >= (const Timestamp& ts) const;
	bool operator <  (const Timestamp& ts) const;
    bool operator <= (const Timestamp& ts) const;
    
    bool operator == (TimeVal ts) const;
	bool operator != (TimeVal ts) const;
	bool operator >  (TimeVal ts) const;
	bool operator >= (TimeVal ts) const;
	bool operator <  (TimeVal ts) const;
	bool operator <= (TimeVal ts) const;
	
	Timestamp  operator +  (TimeDiff d) const;
	Timestamp  operator +  (const Timespan& span) const;
	Timestamp  operator -  (TimeDiff d) const;
	Timestamp  operator -  (const Timespan& span) const;
	TimeDiff   operator -  (const Timestamp& ts) const;
	Timestamp& operator += (TimeDiff d);
	Timestamp& operator += (const Timespan& span);
	Timestamp& operator -= (TimeDiff d);
	Timestamp& operator -= (const Timespan& span);
    
    /** 
     * @brief  convert to struct timespec
     * @note   
     * @retval None
     */
    operator timespec() const;
    /** 
     * @brief  
     * @note   
     * @retval 
     */
    std::time_t second() const;

    /** 
     * @brief  
     * @note   
     * @retval 
     */
    UtcTimeVal utc_time() const;

    /** 
     * @brief  
     * @note   
     * @retval 
     */
    TimeVal microsecond() const;

    /** 
     * @brief  
     * @note   
     * @retval 
     */
    TimeDiff elapsed() const;

    /** 
     * @brief  get local struct tm
     * @note   
     * @retval 
     */
    std::tm local_time() const;

    std::tm gmtime() const;
    /** 
     * @brief  
     * @note   
     * @param  interval: 
     * @retval 
     */
    bool is_elapsed(TimeDiff interval) const;
    bool is_elapsed(const Timespan &span) const;
    /** 
     * @brief  
     * @note   
     * @retval 
     */
    static TimeDiff ratio();

    /** 
     * @brief  
     * @note   
     * @param  tv: 
     * @retval 
     */
    static Timestamp from_micro_second(TimeVal tv);

    /** 
     * @brief  
     * @note   
     * @param  t: 
     * @retval 
     */
    static Timestamp from_second(std::time_t t);

    /** 
     * @brief  
     * @note   
     * @param  ut: 
     * @retval 
     */
    static Timestamp from_utc_time(UtcTimeVal ut);

    static Timestamp max();

    static Timestamp min();

private:
    TimeVal    _ts;
};

/** 
 * @brief  trans int type to Timeval type
 * @note   
 * @param  s: 
 * @retval 
 */
template <typename Source>
WALLE_FORCE_INLINE Timestamp::TimeVal timeval_cast(Source s)
{
    WALLE_STATIC_ASSERT(std::is_integral<Source>::value, "source should be integer");
    return static_cast<Timestamp::TimeVal>(s);
}

template <typename Source>
WALLE_FORCE_INLINE Timestamp::TimeDiff timediff_cast(Source s)
{
    WALLE_STATIC_ASSERT(std::is_integral<Source>::value, "source should be integer");
    return static_cast<Timestamp::TimeDiff>(s);
}

//
//inlines
//

WALLE_FORCE_INLINE bool Timestamp::operator == (const Timestamp& ts) const
{
	return _ts == ts._ts;
}

WALLE_FORCE_INLINE bool Timestamp::operator != (const Timestamp& ts) const
{
	return _ts != ts._ts;
}


WALLE_FORCE_INLINE bool Timestamp::operator >  (const Timestamp& ts) const
{
	return _ts > ts._ts;
}


WALLE_FORCE_INLINE bool Timestamp::operator >= (const Timestamp& ts) const
{
	return _ts >= ts._ts;
}


WALLE_FORCE_INLINE bool Timestamp::operator <  (const Timestamp& ts) const
{
	return _ts < ts._ts;
}


WALLE_FORCE_INLINE bool Timestamp::operator <= (const Timestamp& ts) const
{
	return _ts <= ts._ts;
}

WALLE_FORCE_INLINE bool Timestamp::operator == (Timestamp::TimeVal ts) const
{
    return _ts == ts;
}
WALLE_FORCE_INLINE bool Timestamp::operator != (Timestamp::TimeVal ts) const
{
    return _ts != ts;
}
WALLE_FORCE_INLINE bool Timestamp::operator >  (Timestamp::TimeVal ts) const
{
    return _ts > ts;
}
WALLE_FORCE_INLINE bool Timestamp::operator >= (Timestamp::TimeVal ts) const
{
    return _ts >= ts;
}
WALLE_FORCE_INLINE bool Timestamp::operator <  (Timestamp::TimeVal ts) const
{
    return _ts < ts;
}
WALLE_FORCE_INLINE bool Timestamp::operator <= (Timestamp::TimeVal ts) const
{
    return _ts <= ts;
}
    

WALLE_FORCE_INLINE Timestamp Timestamp::operator + (Timestamp::TimeDiff d) const
{
	return Timestamp(_ts + d);
}


WALLE_FORCE_INLINE Timestamp Timestamp::operator - (Timestamp::TimeDiff d) const
{
	return Timestamp(_ts - d);
}


WALLE_FORCE_INLINE Timestamp::TimeDiff Timestamp::operator - (const Timestamp& ts) const
{
	return _ts - ts._ts;
}


WALLE_FORCE_INLINE Timestamp& Timestamp::operator += (Timestamp::TimeDiff d)
{
	_ts += d;
	return *this;
}


WALLE_FORCE_INLINE Timestamp& Timestamp::operator -= (Timestamp::TimeDiff d)
{
	_ts -= d;
	return *this;
}

WALLE_FORCE_INLINE std::time_t Timestamp::second() const
{
	return std::time_t(_ts/ratio());
}


WALLE_FORCE_INLINE Timestamp::UtcTimeVal Timestamp::utc_time() const
{
	return _ts*10 + (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
}


WALLE_FORCE_INLINE Timestamp::TimeVal Timestamp::microsecond() const
{
	return _ts;
}


WALLE_FORCE_INLINE Timestamp::TimeDiff Timestamp::elapsed() const
{
	Timestamp now;
	return now - *this;
}


WALLE_FORCE_INLINE bool Timestamp::is_elapsed(Timestamp::TimeDiff interval) const
{
    Timestamp now;
    return (now - *this) >= interval;
}

WALLE_FORCE_INLINE Timestamp::TimeDiff Timestamp::ratio()
{
	return 1000000;
}

WALLE_FORCE_INLINE Timestamp::operator timespec() const
{
    struct timespec ts;
    ts.tv_sec = _ts / ratio();
    ts.tv_nsec = (_ts % ratio());
    return ts;
}

/** 
 * @brief  swap
 * @note   
 * @param  s1: 
 * @param  s2: 
 * @retval None
 */
WALLE_FORCE_INLINE void swap(Timestamp& s1, Timestamp& s2)
{
	s1.swap(s2);
}

} //namespace walle
#endif //WALLE_TIME_TIMESTAMP_H_