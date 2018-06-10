#ifndef WALLE_TIME_DATE_TIME_H_
#define WALLE_TIME_DATE_TIME_H_
#include <walle/time/timestamp.h>
#include <walle/config/base.h>

namespace walle {

enum class Months{
    eJanuary = 1,
    eFebruary,
    eMarch,
    eApril,
    eMay,
    eJune,
    eJuly,
    eAugest,
    eSeptember,
    eOctober,
    eNovember,
    eDecember
};

enum class WeekDays {
    eSunday,
    eMonday,
    eTuesday,
    eWedesday,
    eThusday,
    eFriday,
    eSaturday
};

class DateTime{
public: 
    /** 
     * @brief  Creates a DateTime for the current date and time.
     * @note   
     * @retval 
     */
    DateTime();

    /** 
     * @brief  Creates a DateTime for the date and time given in
	 * a Timestamp.
     * @note   
     * @param  &ts: 
     * @retval 
     */
    DateTime(const Timestamp &ts);

    /** 
     * @brief  Creates a DateTime for the given Gregorian date and time.
     * @note   
     * @param  year:  from 0 to 9999.
     * @param  month: from 1 to 12.
     * @param  day: from 1 to 31.
     * @param  hour: from 0 to 23.
     * @param  minute: from 0 to 59.
     * @param  second: from 0 to 59.
     * @param  millisecond: from 0 to 999.
     * @param  microsecond:  from 0 to 999.
     * @retval 
     */
    DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

    /** 
     * @brief   Creates a DateTime for the given Julian day.
     * @note   
     * @param  julianDay: 
     * @retval 
     */
    DateTime(double julianDay);

    /** 
     * @brief   Creates a DateTime from an UtcTimeVal and a TimeDiff.
     * @note   Mainly used internally by DateTime and friends.	
     * @param  utcTime: 
     * @param  diff: 
     * @retval 
     */
    DateTime(Timestamp::UtcTimeVal utcTime, Timestamp::TimeDiff diff);

    /** 
     * @brief  Copy constructor. Creates the DateTime from another one.
     * @note   
     * @param  &rhs: 
     * @retval 
     */
    DateTime(const DateTime &rhs);

    /** 
     * @brief   Destroys the DateTime.
     * @note   
     * @retval 
     */
    ~DateTime();

    /** 
     * @brief  Assigns another DateTime.
     * @note   
     * @param  dateTime: 
     * @retval None
     */
    DateTime& operator = (const DateTime& dateTime);

    /** 
     * @brief  Assigns a Timestamp.
     * @note   
     * @param  timestamp: 
     * @retval None
     */
    DateTime& operator = (const Timestamp& timestamp);

    /** 
     * @brief  Assigns a Julian day.
     * @note   
     * @param  julianDay: 
     * @retval None
     */
    DateTime& operator = (double julianDay);

    /** 
     * @brief  
     * @note   
     * @param  year:  from 0 to 9999.
     * @param  month: from 1 to 12.
     * @param  day: from 1 to 31.
     * @param  hour: from 0 to 23.
     * @param  minute: from 0 to 59.
     * @param  second: from 0 to 59.
     * @param  millisecond: from 0 to 999.
     * @param  microsecond:  from 0 to 999.
     * @retval 
     */
    DateTime& assign(int year, int month, int day, 
                    int hour = 0, int minute = 0, int second = 0, 
                    int millisecond = 0, int microseconds = 0);
    
    /** 
     * @brief  Swaps the DateTime with another one.
     * @note   
     * @param  dateTime: 
     * @retval None
     */
    void swap(DateTime& dateTime);
    
    /** 
     * @brief  return the year.
     * @note   
     * @retval 
     */
    int year() const;
    
    /** 
     * @brief  return the month.
     * @note   
     * @retval 
     */
    int month() const;
    
    /** 
     * @brief return the week number within the year.
     * @note  FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1).
	 * The returned week number will be from 0 to 53. Week number 1 is the week 
	 * containing January 4. This is in accordance to ISO 8601.
	 * 
	 * The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started
	 * on a Saturday, week 1 will be the week starting on Monday, January 3.
	 * January 1 and 2 will fall within week 0 (or the last week of the previous year).
	 *
	 * For 2007, which starts on a Monday, week 1 will be the week starting on Monday, January 1.
	 * There will be no week 0 in 2007. 
     * @retval 
     */
    int week(WeekDays firstDayOfWeek = WeekDays::eMonday) const;

    /** 
     * @brief  return the day within month(1-31)
     * @note   
     * @retval 
     */
    int day() const;
    
    /** 
     * @brief  return the day of week(0-6)
     * @note   
     * @retval 
     */
    int day_of_week() const;
    
    /** 
     * @brief  return the day of year
     * @note   
     * @retval 
     */
    int day_of_year() const;
    
    /** 
     * @brief  return hour of day(0-23)
     * @note   
     * @retval 
     */
    int hour() const;
    
    /** 
     * @brief  return the minutes of hour(0-59)
     * @note   
     * @retval 
     */
    int minute() const;

    /** 
     * @brief   the hour (0 to 12).
     * @note   
     * @retval 
     */
    bool is_am() const;
    
    /** 
     * @brief   true if hour >= 12.
     * @note   
     * @retval 
     */
    bool is_pm() const;

    /** 
     * @brief  the hour (0 to 12).
     * @note   
     * @retval 
     */
    int hour_am_pm() const;
    
    /** 
     * @brief  return  the second (0 to 59).
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
     * @brief  the microsecond (0 to 999)
     * @note   
     * @retval 
     */
    int microsecond() const;

    /** 
     * @brief  the julian day for the date and time.
     * @note   
     * @retval 
     */
    double julianday() const;

    /** 
     * @brief  the date and time expressed as a Timestamp.
     * @note   
     * @retval 
     */
    Timestamp timestamp() const;

    Timestamp::UtcTimeVal utc_time() const;

	bool operator == (const DateTime& dateTime) const;
	bool operator != (const DateTime& dateTime) const;
	bool operator <  (const DateTime& dateTime) const;
	bool operator <= (const DateTime& dateTime) const;
	bool operator >  (const DateTime& dateTime) const;
	bool operator >= (const DateTime& dateTime) const;

	DateTime  operator +  (const Timespan& span) const;
	DateTime  operator -  (const Timespan& span) const;
	Timespan  operator -  (const DateTime& dateTime) const;
	DateTime& operator += (const Timespan& span);
    DateTime& operator -= (const Timespan& span);
    
    /** 
     * @brief  Converts a local time into UTC, by applying the given 
     * time zone differential.	
     * @note   
     * @param  tzd: 
     * @retval None
     */
    void make_utc(int tzd);

    /** 
     * @brief  Converts a UTC time into a local time, by applying the
     * given time zone differential.
     * @note   
     * @param  tzd: 
     * @retval None
     */
    void make_local(int tzd);

    /** 
     * @brief  eturn true if the given year is a leap year;
	 * false otherwise.
     * @note   
     * @param  year: 
     * @retval 
     */
    static bool is_leap_year(int year);
    
    /** 
     * @brief  eturn the number of days in the given month
	 * and year. Month is from 1 to 12.
     * @note   
     * @param  year: 
     * @param  month: 
     * @retval None
     */
    static int days_of_month(int year, int month);

    /** 
     * @brief  Checks if the given date and time is valid
	 * (all arguments are within a proper range).
     * @note   
     * @param  year: 
     * @param  month: 
     * @param  day: 
     * @param  hour: 
     * @param  minute: 
     * @param  second: 
     * @param  millisecond: 
     * @param  microsecond: 
     * @retval 
     */
    static bool is_valid(int year, int month, int day, 
                        int hour = 0, int minute = 0, int second = 0, 
                        int millisecond = 0, int microsecond = 0);

protected:
    
    /** 
     * @brief  Computes the Julian day for an UTC time.
     * @note   
     * @param  utcTime: 
     * @retval 
     */
    static double to_julianday(Timestamp::UtcTimeVal utcTime);  
    
    /** 
     * @brief  Computes the Julian day for a Gregorian calendar date and time.
	 * See <http://vsg.cape.com/~pbaum/date/jdimp.htm>, section 2.3.1 for the algorithm.
     * @note   
     * @param  year: 
     * @param  month: 
     * @param  day: 
     * @param  hour: 
     * @param  minute: 
     * @param  second: 
     * @param  millisecond: 
     * @param  microsecond: 
     * @retval 
     */
    static double to_julianday(int year, int month, int day, 
        int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
    
    /** 
     * @brief  Computes the UTC time for a Julian day.
     * @note   
     * @param  julianDay: 
     * @retval 
     */
    static Timestamp::UtcTimeVal to_utc_time(double julianDay);
     
    /** 
     * @brief Computes the Gregorian date for the given Julian day.
	 * See <http://vsg.cape.com/~pbaum/date/injdimp.htm>, section 3.3.1 for the algorithm.
     * @note   
     * @param  julianDay: 
     * @retval None
     */
    void compute_gregorian(double julianDay);

    /** 
     * @brief  Extracts the daytime (hours, minutes, seconds, etc.) from the stored utcTime.
     * @note   
     * @retval None
     */
    void compute_daytime();
private:

    void check_limit(short& lower, short& higher, short limit);

	/** 
	 * @brief  utility functions used to correct the overflow in computeGregorian
	 * @note   
	 * @retval None
	 */
    void normalize();
private:
    Timestamp::UtcTimeVal  _utc_time;
	short                  _year;
	short                  _month;
	short                  _day;
	short                  _hour;
	short                  _minute;
	short                  _second;
	short                  _millisecond;
	short                  _microsecond;
};

//
// inlines
//

WALLE_FORCE_INLINE Timestamp DateTime::timestamp() const
{
    return Timestamp::from_utc_time(_utc_time);
}

WALLE_FORCE_INLINE Timestamp::UtcTimeVal DateTime::utc_time() const
{
    return _utc_time;
}

WALLE_FORCE_INLINE int DateTime::year() const
{
    return _year;
}

WALLE_FORCE_INLINE int DateTime::month() const
{
    return _month;
}

WALLE_FORCE_INLINE int DateTime::day() const
{
    return _day;
}

WALLE_FORCE_INLINE int DateTime::hour() const
{
    return _hour;
}

WALLE_FORCE_INLINE int DateTime::hour_am_pm() const
{
    if (_hour < 1) {
        return 12;
    } else if (_hour > 12) {
        return _hour - 12;
    } else {
        return _hour;
    }
}

WALLE_FORCE_INLINE bool DateTime::is_am() const
{
	return _hour < 12;
}

WALLE_FORCE_INLINE bool DateTime::is_pm() const
{
	return _hour >= 12;
}

WALLE_FORCE_INLINE int DateTime::minute() const
{
	return _minute;
}

WALLE_FORCE_INLINE int DateTime::second() const
{
	return _second;
}

	
WALLE_FORCE_INLINE int DateTime::millisecond() const
{
	return _millisecond;
}


WALLE_FORCE_INLINE int DateTime::microsecond() const
{
	return _microsecond;
}

WALLE_FORCE_INLINE bool DateTime::operator == (const DateTime& dateTime) const
{
	return _utc_time == dateTime._utc_time;
}


WALLE_FORCE_INLINE bool DateTime::operator != (const DateTime& dateTime) const
{
	return _utc_time != dateTime._utc_time;
}


WALLE_FORCE_INLINE bool DateTime::operator < (const DateTime& dateTime) const
{
	return _utc_time < dateTime._utc_time;
}


WALLE_FORCE_INLINE bool DateTime::operator <= (const DateTime& dateTime) const
{
	return _utc_time <= dateTime._utc_time;
}


WALLE_FORCE_INLINE bool DateTime::operator > (const DateTime& dateTime) const
{
	return _utc_time > dateTime._utc_time;
}


WALLE_FORCE_INLINE bool DateTime::operator >= (const DateTime& dateTime) const
{
	return _utc_time >= dateTime._utc_time;
}

WALLE_FORCE_INLINE bool DateTime::is_leap_year(int year)
{
	return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}


WALLE_FORCE_INLINE void swap(DateTime& d1, DateTime& d2)
{
	d1.swap(d2);
}

} //namespace walle
#endif //WALLE_TIME_DATE_TIME_H_