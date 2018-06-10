#ifndef WALLE_TIME_CLOCK_H_
#define WALLE_TIME_CLOCK_H_

#include <walle/config/base.h>
#include <cstdint>
#include <algorithm>
#include <time.h>
#include <unistd.h>

namespace walle {

/**
 * @brief nano second 
 * 
 */
enum class ClockType {
    eNon,
    eRealTime,
    eMonotonic,
    eProcessCpuId,
    eThreadCpuId
};

/** 
 * @brief clock value in microsecond resolution.
 */
typedef int64_t ClockVal;

/** 
 * @brief  different between two Clocks in microsecond
 */
typedef int64_t ClockDiff;

template <ClockType Type = ClockType::eRealTime>
class Clock{
public:
    static const ClockVal kClockValMax = 0x7FFFFFFFFFFFFFFF; //!< maximum clock value.
    static const ClockVal kClockValMin = 0; //!< minimum clock value
public:
    /** 
     * @brief  default constructor will use update().
     * @note   
     * @retval 
     */
    Clock();

    /** 
     * @brief  create a Clock from given value.
     * @note   
     * @param  tv: clock value in microsecond.
     * @retval 
     */
    Clock(ClockVal tv);

    /** 
     * @brief  cpoy an other Clock to this
     * @note   
     * @param  &other: Clock object
     * @retval 
     */
    Clock(const Clock &other);

    /** 
     * @brief  destructor, no operation.
     * @note   
     * @retval 
     */
    ~Clock();

    /** 
     * @brief  assign operator
     * @note   you can use this to trans two different
     *         type.
     * @param  &other: 
     * @retval None
     */
    Clock& operator = (const Clock &other);
    template <typename IntType>
    Clock& operator = (IntType  tv);

    /** 
     * @brief  swap two Clock
     * @note   
     * @param  &other: 
     * @retval None
     */
    void swap(Clock &other);

    /** 
     * @brief  update stored clockval to now.
     * @note   
     * @retval None
     */
    void update();

    operator timespec() const;
    bool operator == (const Clock& ts) const;
	bool operator != (const Clock& ts) const;
	bool operator >  (const Clock& ts) const;
	bool operator >= (const Clock& ts) const;
	bool operator <  (const Clock& ts) const;
    bool operator <= (const Clock& ts) const;
    
    template <typename IntType>
    bool operator == (const IntType& ts) const;
    template <typename IntType>
    bool operator != (const IntType& ts) const;
    template <typename IntType>
    bool operator >  (const IntType& ts) const;
    template <typename IntType>
    bool operator >= (const IntType& ts) const;
    template <typename IntType>
    bool operator <  (const IntType& ts) const;
    template <typename IntType>
    bool operator <= (const IntType& ts) const;

	template <typename IntType>
    Clock  operator +  (const IntType &d) const;
    Clock  operator +  (const Clock &ts) const;
    
    template <typename IntType>
    Clock  operator *  (const IntType &d) const;

    template <typename IntType>
    Clock  operator /  (const IntType &d) const;

    template <typename IntType>
    Clock  operator %  (const IntType &d) const;

    template <typename IntType>
    Clock  operator -  (const IntType &d) const;
    Clock  operator -  (const Clock &ts) const;
    
    template <typename IntType>
    Clock& operator += (const IntType &d);
    Clock& operator += (const Clock &ts);

    template <typename IntType>
    Clock& operator -= (const IntType &d);
    Clock& operator -= (const Clock &ts);

    template <typename IntType>
    Clock& operator *= (const IntType &d);

    template <typename IntType>
    Clock& operator /= (const IntType &d);

    template <typename IntType>
    Clock& operator %= (const IntType &d);

    ClockVal micoseconds() const;

    ClockVal raw() const;
    
    ClockDiff elapsed() const;

    bool is_elapsed(ClockDiff interval) const;

    static ClockDiff ratio();

    static ClockDiff accuracy();

    static ClockType clock_type();
private:
    static clockid_t clock_type_to_system();
private:
    clockid_t        _type;
    ClockVal         _clock;
};

// 
// inlines
//

template <ClockType Type>
WALLE_FORCE_INLINE Clock<Type>::Clock()
    : _type(clock_type_to_system()),
      _clock(0)
{
    WALLE_STATIC_ASSERT((Type == ClockType::eRealTime ||
                   Type == ClockType::eMonotonic ||
                   Type == ClockType::eProcessCpuId ||
                   Type == ClockType::eThreadCpuId), "not known type");
    
    update();
}

template <ClockType Type>
WALLE_FORCE_INLINE clockid_t Clock<Type>::clock_type_to_system()
{
    switch(Type) {
        case ClockType::eRealTime :
            return CLOCK_REALTIME;
        case ClockType::eMonotonic :
            return CLOCK_MONOTONIC;
        case ClockType::eProcessCpuId:
            return CLOCK_PROCESS_CPUTIME_ID;
        case ClockType::eThreadCpuId:
            return CLOCK_THREAD_CPUTIME_ID;
        default:
            return CLOCK_MONOTONIC;
    }
}
template <ClockType Type>
WALLE_FORCE_INLINE Clock<Type>::~Clock()
{
    
}

template <ClockType Type>
WALLE_FORCE_INLINE Clock<Type>::Clock(const Clock &other)
{
    _clock = other._clock;
}
template <ClockType Type>
WALLE_FORCE_INLINE Clock<Type>& Clock<Type>::operator = (const Clock<Type>& ts)
{
    _clock = ts._clock;
    _type  = ts._type;
    return *this;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type>& Clock<Type>::operator = (IntType ts)
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    _clock = ts;
    _type = Type;
    return *this;
}

template <ClockType Type>
WALLE_FORCE_INLINE bool Clock<Type>::operator == (const Clock& ts) const
{
	return _clock == ts._clock;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE bool Clock<Type>::operator ==(const IntType &v) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    return _clock == static_cast<int64_t>(v);
}

template <ClockType Type>
WALLE_FORCE_INLINE bool Clock<Type>::operator != (const Clock& ts) const
{
	return _clock != ts._clock;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE bool Clock<Type>::operator != (const IntType &v) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    return _clock != static_cast<int64_t>(v);
}

template <ClockType Type>
WALLE_FORCE_INLINE bool Clock<Type>::operator > (const Clock& ts) const
{
	return _clock > ts._clock;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE bool Clock<Type>::operator > (const IntType &v) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    return _clock > static_cast<int64_t>(v);
}

template <ClockType Type>
WALLE_FORCE_INLINE bool Clock<Type>::operator >= (const Clock& ts) const
{
	return _clock >= ts._clock;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE bool Clock<Type>::operator >= (const IntType &v) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    return _clock >= static_cast<int64_t>(v);
}

template <ClockType Type>
WALLE_FORCE_INLINE bool Clock<Type>::operator < (const Clock& ts) const
{
	return _clock < ts._clock;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE bool Clock<Type>::operator < (const IntType &v) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    return _clock < static_cast<int64_t>(v);
}

template <ClockType Type>
WALLE_FORCE_INLINE bool Clock<Type>::operator <= (const Clock& ts) const
{
	return _clock <= ts._clock;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE bool Clock<Type>::operator <= (const IntType &v) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    return _clock <= static_cast<int64_t>(v);
}

template <ClockType Type>
WALLE_FORCE_INLINE Clock<Type> Clock<Type>::operator + (const Clock<Type> &ts) const
{
    ClockVal clock = _clock + ts._clock;
    return Clock<Type>(clock);
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type> Clock<Type>::operator + (const IntType &ts) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    ClockVal clock = _clock + static_cast<int64_t>(ts);
    return Clock<Type>(clock);
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type> Clock<Type>::operator * (const IntType &ts) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    ClockVal clock = _clock * static_cast<int64_t>(ts);
    return Clock<Type>(clock);
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type> Clock<Type>::operator / (const IntType &ts) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    ClockVal clock = _clock / static_cast<int64_t>(ts);
    return Clock<Type>(clock);
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type> Clock<Type>::operator % (const IntType &ts) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    ClockVal clock = _clock % static_cast<int64_t>(ts);
    return Clock<Type>(clock);
}

template <ClockType Type>
WALLE_FORCE_INLINE Clock<Type> Clock<Type>::operator - (const Clock<Type> &ts) const
{
    ClockVal clock = _clock + ts._clock;
    WALLE_ASSERT(clock > 0, "clock shoud be > 0");
    return Clock<Type>(clock);
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type> Clock<Type>::operator - (const IntType &ts) const
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    ClockVal clock = _clock - static_cast<int64_t>(ts);
    WALLE_ASSERT(clock >= 0, "clock shoud be > 0");
    return Clock<Type>(clock);
}

template <ClockType Type>
Clock<Type>& Clock<Type>::operator += (const Clock<Type> &ts)
{
    _clock += ts._clock;
    return *this;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type>& Clock<Type>::operator += (const IntType &ts)
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    _clock += static_cast<int64_t>(ts);
    WALLE_ASSERT(_clock >= 0, "clock shoud be > 0");
    return *this;
}

template <ClockType Type>
Clock<Type>& Clock<Type>::operator -= (const Clock<Type> & ts)
{
    _clock -= ts._clock;
    WALLE_ASSERT(_clock >= 0, "clock shoud be > 0");
    return *this;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type>& Clock<Type>::operator -= (const IntType &ts)
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    _clock -= static_cast<int64_t>(ts);
    WALLE_ASSERT(_clock >= 0, "clock shoud be > 0");
    return *this;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type>& Clock<Type>::operator *= (const IntType &ts)
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    _clock *= static_cast<int64_t>(ts);
    WALLE_ASSERT(_clock >= 0, "clock shoud be > 0");
    return *this;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type>& Clock<Type>::operator /= (const IntType &ts)
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    _clock /= static_cast<int64_t>(ts);
    WALLE_ASSERT(_clock >= 0, "clock shoud be > 0");
    return *this;
}

template <ClockType Type>
template <typename IntType>
WALLE_FORCE_INLINE Clock<Type>& Clock<Type>::operator %= (const IntType &ts)
{
    WALLE_STATIC_ASSERT(std::is_integral<IntType>::value, "parameter should be integral types");
    _clock *= static_cast<int64_t>(ts);
    WALLE_ASSERT(_clock >= 0, "clock shoud be > 0");
    return *this;
}

template <ClockType Type>
WALLE_FORCE_INLINE ClockVal Clock<Type>::micoseconds() const
{
    return _clock;
}

template <ClockType Type>
WALLE_FORCE_INLINE ClockVal Clock<Type>::raw() const
{
    return _clock;
}

template <ClockType Type>
WALLE_FORCE_INLINE ClockVal Clock<Type>::ratio()
{
    return 1000000;
}

template <ClockType Type>
WALLE_FORCE_INLINE void Clock<Type>::swap(Clock<Type> &other)
{
    ClockVal tmp = _clock;
    _clock = other._clock;
    other._clock = tmp;
}

template <ClockType Type>
WALLE_FORCE_INLINE ClockType Clock<Type>::clock_type()
{
    return Type;
}

template <ClockType Type>
WALLE_FORCE_INLINE ClockDiff Clock<Type>::accuracy()
{
    struct timespec ts;
	::clock_getres(clock_type_to_system(), &ts);
	ClockVal acc = ClockVal(ts.tv_sec)*ratio() + ts.tv_nsec/1000;
	return acc > 0 ? acc : 1;
}

template <ClockType Type>
WALLE_FORCE_INLINE ClockDiff Clock<Type>::elapsed() const
{
    Clock<Type> now;
    now -= *this;
    return now._clock;
}

template <ClockType Type>
WALLE_FORCE_INLINE bool Clock<Type>::is_elapsed(ClockDiff interval) const
{
    Clock<Type> now;
    now -= *this;
    return now._clock >= interval;
}

template <ClockType Type>
WALLE_FORCE_INLINE void Clock<Type>::update()
{
	struct timespec ts;
	::clock_gettime(_type, &ts);
	_clock = ClockVal(ts.tv_sec)*ratio() + ts.tv_nsec/1000;
}

template <ClockType Type>
Clock<Type>::operator timespec() const
{
    struct timespec ts;
    ts.tv_sec = _clock / ratio();
    ts.tv_nsec = (_clock % ratio()) * 1000;
    return ts;
}

typedef Clock<ClockType::eRealTime>      RealTimeClock;
typedef Clock<ClockType::eMonotonic>     MonotonicTimeClock;
typedef Clock<ClockType::eProcessCpuId>  ProcessTimeClock;
typedef Clock<ClockType::eThreadCpuId>   ThreadTimeClock;

} //namespace walle

#endif //WALLE_TIME_CLOCK_H_