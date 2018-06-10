#ifndef WALLE_TIME_LOCAL_DATE_TIME_H_
#define WALLE_TIME_LOCAL_DATE_TIME_H_
#include <walle/time/date_time.h>
#include <walle/time/timestamp.h>
#include <walle/time/timespan.h>

namespace walle {

class LocalDateTime{
public:
    /** 
     * @brief Creates a LocalDateTime with the current date/time 
	 *        for the current time zone.
     * @note   
     * @retval 
     */
    LocalDateTime();

    /** 
     * @brief Creates a LocalDateTime for the given Gregorian local date and time.
     * @note   
     * @param  year: year is from 0 to 9999.
     * @param  month:  month is from 1 to 12.
     * @param  day: day is from 1 to 31.
     * @param  hour: hour is from 0 to 23.
     * @param  minute: minute is from 0 to 59.
     * @param  second: second is from 0 to 59.
     * @param  millisecond: millisecond is from 0 to 999.
     * @param  microsecond: microsecond is from 0 to 999.
     * @retval 
     */
    LocalDateTime(int year, int month, int day, 
                  int hour = 0, int minute = 0, 
                  int second = 0, int millisecond = 0, int microsecond = 0);
   /** 
     * @brief Creates a LocalDateTime for the given Gregorian local date and time.
     * @note
     * @param  tzd: tzd is in seconds.
     * @param  year: year is from 0 to 9999.
     * @param  month:  month is from 1 to 12.
     * @param  day: day is from 1 to 31.
     * @param  hour: hour is from 0 to 23.
     * @param  minute: minute is from 0 to 59.
     * @param  second: second is from 0 to 59.
     * @param  millisecond: millisecond is from 0 to 999.
     * @param  microsecond: microsecond is from 0 to 999.
     * @retval 
     */
    LocalDateTime(int tzd, int year, int month, int day, 
                  int hour, int minute, int second, 
                  int millisecond, int microsecond);
	
    /** 
     * @brief  Creates a LocalDateTime from the UTC time given in dateTime,
     *         using the time zone differential of the current time zone.
     * @note   
     * @param  dateTime: 
     * @retval 
     */
    LocalDateTime(const DateTime& dateTime);
    
    /** 
     * @brief  Creates a LocalDateTime from the UTC time given in dateTime,
	 *         using the given time zone differential. Adjusts dateTime
	 *         for the given time zone differential.
     * @note   
     * @param  tzd: 
     * @param  dateTime: 
     * @retval 
     */
    LocalDateTime(int tzd, const DateTime& dateTime);
    
    /** 
     * @brief  Creates a LocalDateTime from the UTC time given in dateTime,
     *         using the given time zone differential. If adjust is true, 
     *         adjusts dateTime for the given time zone differential.
     * @note   
     * @param  tzd: 
     * @param  dateTime: 
     * @param  adjust: 
     * @retval 
     */
    LocalDateTime(int tzd, const DateTime& dateTime, bool adjust);
    
    /** 
     * @brief  
     * @note   
     * @param  julianDay: 
     * @retval 
     */
    LocalDateTime(double julianDay);

    /** 
     * @brief  Creates a LocalDateTime for the given Julian day in the time zone
	 *         denoted by the time zone differential in tzd.
     * @note   
     * @param  tzd: 
     * @param  julianDay: 
     * @retval 
     */
    LocalDateTime(int tzd, double julianDay);
    
    /** 
     * @brief  Copy constructor. Creates the LocalDateTime from another one.
     * @note   
     * @param  dateTime: 
     * @retval 
     */
    LocalDateTime(const LocalDateTime& dateTime);

    /** 
     * @brief  Destroys the LocalDateTime.
     * @note   
     * @retval 
     */
    ~LocalDateTime();
        
    /** 
     * @brief   Assigns another LocalDateTime.
     * @note   
     * @param  dateTime: 
     * @retval None
     */
    LocalDateTime& operator = (const LocalDateTime& dateTime);

    /** 
     * @brief Assigns a timestamp
     * @note   
     * @param  timestamp: 
     * @retval None
     */
    LocalDateTime& operator = (const Timestamp& timestamp);

    /** 
     * @brief  Assigns a Julian day in the local time zone.
     * @note   
     * @param  julianDay: 
     * @retval None
     */
    LocalDateTime& operator = (double julianDay);
    
    /** 
     * @brief Assigns a Gregorian local date and time.
     * @note
     * @param  year: year is from 0 to 9999.
     * @param  month:  month is from 1 to 12.
     * @param  day: day is from 1 to 31.
     * @param  hour: hour is from 0 to 23.
     * @param  minute: minute is from 0 to 59.
     * @param  second: second is from 0 to 59.
     * @param  millisecond: millisecond is from 0 to 999.
     * @param  microsecond: microsecond is from 0 to 999.
     * @retval 
     */
    LocalDateTime& assign(int year, int month, int day, 
                          int hour = 0, int minute = 0, 
                          int second = 0, int millisecond = 0, 
                          int microseconds = 0);
    
    /** 
     * @brief Assigns a Gregorian local date and time.
     * @note
     * @param  tzd: tzd is in seconds.
     * @param  year: year is from 0 to 9999.
     * @param  month:  month is from 1 to 12.
     * @param  day: day is from 1 to 31.
     * @param  hour: hour is from 0 to 23.
     * @param  minute: minute is from 0 to 59.
     * @param  second: second is from 0 to 59.
     * @param  millisecond: millisecond is from 0 to 999.
     * @param  microsecond: microsecond is from 0 to 999.
     * @retval 
     */
    LocalDateTime& assign(int tzd, int year, int month, int day, 
                          int hour, int minute, int second, 
                          int millisecond, int microseconds);

    /** 
     * @brief  Assigns a Julian day in the time zone denoted by the
	 *         time zone differential in tzd.
     * @note   
     * @param  tzd: 
     * @param  julianDay: 
     * @retval 
     */
    LocalDateTime& assign(int tzd, double julianDay);
    
    /** 
     * @brief  Swaps the LocalDateTime with another one.
     * @note   
     * @param  dateTime: 
     * @retval None
     */
    void swap(LocalDateTime& dateTime);
    
    /** 
     * @brief  return the year.
     * @note   
     * @retval 
     */
    int year() const;

    /** 
     * @brief  return the month(1-12)
     * @note   
     * @retval 
     */
    int month() const;
    
    /** 
     * @brief  the week number within the year.
     * @note   
     * @retval 
     */
    int week(WeekDays firstDayOfWeek = WeekDays::eMonday) const;

    /** 
     * @brief  the day within the month (1 to 31).
     * @note   
     * @retval 
     */
    int day() const;

    /** 
     * @brief  the weekday (0 to 6, where
	 *         0 = Sunday, 1 = Monday, ..., 6 = Saturday).
     * @note   
     * @retval 
     */
    int day_of_week() const;

    /** 
     * @brief   the number of the day in the year.
     * @note   January 1 is 1, February 1 is 32, etc.
     * @retval 
     */
    int day_of_year() const;

    /** 
     * @brief  the hour (0 to 23).
     * @note   
     * @retval 
     */
    int hour() const;

    /** 
     * @brief hour (0 to 12).
     * @note   
     * @retval 
     */
    int hour_am_pm() const;
    
    /** 
     * @brief  true if hour < 12;
     * @note   
     * @retval 
     */
    bool is_am() const;

    /** 
     * @brief true if hour >= 12.
     * @note   
     * @retval 
     */
    bool is_pm() const;

    /** 
     * @brief  the minute (0 to 59).
     * @note   
     * @retval 
     */
    int minute() const;

    /** 
     * @brief the second (0 to 59).
     * @note   
     * @retval 
     */
    int second() const;

    /** 
     * @brief  the millisecond (0 to 999)
     * @note   
     * @retval 
     */
    int millisecond() const;

    /** 
     * @brief the microsecond (0 to 999)
     * @note   
     * @retval 
     */
    int microsecond() const;

    /** 
     * @brief  return the Julian day for the date.
     * @note   
     * @retval 
     */
    double julianday() const;

    /** 
     * @brief  the time zone differential.
     * @note   
     * @retval 
     */
    int time_zone_differential() const;

    /** 
     * @brief  the UTC equivalent for the local date and time.
     * @note   
     * @retval 
     */
    DateTime utc() const;

    /** 
     * @brief  the date and time expressed as a Timestamp.
     * @note   
     * @retval 
     */
    Timestamp timestamp() const;

    /** 
     * @brief  the UTC equivalent for the local date and time.
     * @note   
     * @retval 
     */
    Timestamp::UtcTimeVal utc_time() const;

    bool operator == (const LocalDateTime& dateTime) const;	
	bool operator != (const LocalDateTime& dateTime) const;	
	bool operator <  (const LocalDateTime& dateTime) const;	
	bool operator <= (const LocalDateTime& dateTime) const;	
	bool operator >  (const LocalDateTime& dateTime) const;	
    bool operator >= (const LocalDateTime& dateTime) const;	
    
    LocalDateTime  operator +  (const Timespan& span) const;
	LocalDateTime  operator -  (const Timespan& span) const;
	Timespan       operator -  (const LocalDateTime& dateTime) const;
	LocalDateTime& operator += (const Timespan& span);
    LocalDateTime& operator -= (const Timespan& span);
protected:
    LocalDateTime(Timestamp::UtcTimeVal utcTime, 
                  Timestamp::TimeDiff diff, int tzd);
                
    void determine_time_zone_differential(bool adjust = false);

    void adjust_for_time_zone_differential();

    std::time_t day_light_saveing_time_offset(int& dstOffset) const;
private:
    DateTime _dateTime;
	int      _tzd;
};

//
// inlines
//

WALLE_FORCE_INLINE int LocalDateTime::year() const
{
	return _dateTime.year();
}

WALLE_FORCE_INLINE int LocalDateTime::month() const
{
	return _dateTime.month();
}


WALLE_FORCE_INLINE int LocalDateTime::week(WeekDays firstDayOfWeek) const
{
	return _dateTime.week(firstDayOfWeek);
}


WALLE_FORCE_INLINE int LocalDateTime::day() const
{
	return _dateTime.day();
}

	
WALLE_FORCE_INLINE int LocalDateTime::day_of_week() const
{
	return _dateTime.day_of_week();
}


WALLE_FORCE_INLINE int LocalDateTime::day_of_year() const
{
	return _dateTime.day_of_year();
}


WALLE_FORCE_INLINE int LocalDateTime::hour() const
{
	return _dateTime.hour();
}

	
WALLE_FORCE_INLINE int LocalDateTime::hour_am_pm() const
{
	return _dateTime.hour_am_pm();
}


WALLE_FORCE_INLINE bool LocalDateTime::is_am() const
{
	return _dateTime.is_am();
}


WALLE_FORCE_INLINE bool LocalDateTime::is_pm() const
{
	return _dateTime.is_pm();
}

	
WALLE_FORCE_INLINE int LocalDateTime::minute() const
{
	return _dateTime.minute();
}

	
WALLE_FORCE_INLINE int LocalDateTime::second() const
{
	return _dateTime.second();
}

	
WALLE_FORCE_INLINE int LocalDateTime::millisecond() const
{
	return _dateTime.millisecond();
}


WALLE_FORCE_INLINE int LocalDateTime::microsecond() const
{
	return _dateTime.microsecond();
}


WALLE_FORCE_INLINE double LocalDateTime::julianday() const
{
	return _dateTime.julianday();
}


WALLE_FORCE_INLINE int LocalDateTime::time_zone_differential() const
{
	return _tzd;
}


WALLE_FORCE_INLINE Timestamp LocalDateTime::timestamp() const
{
	return Timestamp::from_utc_time(_dateTime.utc_time());
}


WALLE_FORCE_INLINE Timestamp::UtcTimeVal LocalDateTime::utc_time() const
{
	return _dateTime.utc_time() - ((Timestamp::TimeDiff) _tzd)*10000000;
}


WALLE_FORCE_INLINE void LocalDateTime::adjust_for_time_zone_differential()
{
	_dateTime += Timespan(((Timestamp::TimeDiff) _tzd)*Timespan::kSeconds);
}


WALLE_FORCE_INLINE void swap(LocalDateTime& d1, LocalDateTime& d2)
{
	d1.swap(d2);
}

} //namespace walle
#endif //WALLE_TIME_LOCAL_DATE_TIME_H_