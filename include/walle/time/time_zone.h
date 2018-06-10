#ifndef WALLE_TIME_TIME_ZONE_H_
#define WALLE_TIME_TIME_ZONE_H_
#include <walle/time/timestamp.h>
#include <walle/format/format.h>
#include <string>

namespace walle {


class TimeZone {
public:
    static const int kUtc = 0xFFFF;
    /** 
     * @brief  the offset of local time to UTC, in seconds.
     * @note   local time = UTC + utcOffset() + day_light_saving_time().
     * @retval 
     */
    static int utc_offset();

    /** 
     * @brief the daylight saving time offset in seconds if
	 *        daylight saving time is in use. 
     * @note   
     * @retval 
     */
    static int day_light_saving_time();

    /** 
     * @brief  true if daylight saving time is in effect for the given time.
     * @note   Depending on the operating system
	 *         platform this might only work reliably for certain
	 *         date ranges, as the C library's localtime() function
	 *         is used.
     * @retval 
     */
    static bool is_day_light_saving_time(const Timestamp &timestamp);

    /** 
     * @brief  return the time zone differential for the current timezone.
     * @note   
     * @retval 
     */
    static int time_zone_differential();

    /** 
     * @brief  the timezone name currently in effect.
     * @note   
     * @retval 
     */
    static std::string name();

    /** 
     * @brief the timezone name if not daylight saving time is in effect.
     * @note   
     * @retval 
     */
    static std::string standard_name();

    /** 
     * @brief  the timezone name if daylight saving time is in effect.
     * @note   
     * @retval 
     */
    static std::string day_light_saving_time_name();
};

class LocalTimeZoneDiff{
public:
    LocalTimeZoneDiff() :tzd(TimeZone::utc_offset() + TimeZone::day_light_saving_time())
    {
        walle::MemoryWriter out;
        if (tzd != TimeZone::kUtc) {
		    if (tzd >= 0) {
			    out.write("+{:02}.{:02}", tzd/3600, (tzd%3600)/60);
            } else {
			    out.write("-{:02}.{:02}", -tzd/3600, (-tzd%3600)/60);
		    }
	    } else {
            out<<'Z';
        }
        z = out.str();
        out.clear();

        if (tzd != TimeZone::kUtc) {
		    if (tzd >= 0) {
			    out.write("+{:02}{:02}", tzd/3600,(tzd%3600)/60);
		    } else {
			    out.write("-{:02}{:02}", -tzd/3600,(-tzd%3600)/60);
		    }		
	    } else {
            out<<"GMT";
        }
        Z = out.str();

    }
    ~LocalTimeZoneDiff()
    {

    }
public:
    int           tzd;
    std::string   z;
    std::string   Z; 
};

LocalTimeZoneDiff& get_local_tzd();

} // namepace walle
#endif //WALLE_TIME_TIME_ZONE_H_