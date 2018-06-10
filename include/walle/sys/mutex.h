#ifndef WALLE_SYS_MUTEX_H_
#define WALLE_SYS_MUTEX_H_
#include <walle/time/timespan.h>
#include <walle/time/timestamp.h>
#include <walle/sys/detail/mutex_base.h>
#include <walle/sys/condition.h>
#include <algorithm>
#include <pthread.h>

namespace walle {

#if WALLE_PLATEFORM == WALLE_PLATEFORM_LINUX
class TimedMutex {
   typedef pthread_mutex_t NativeType;
    typedef NativeType*     NativeTypePtr;
public:
    TimedMutex() noexcept
    : _mutex(PTHREAD_MUTEX_INITIALIZER)
    {

    }
    ~TimedMutex() {}

    void lock();
    bool try_lock();
    void unlock();
    bool try_lock_for(Timespan span);
    bool try_lock_until(Timestamp stamp);
    NativeTypePtr native_type_ptr() { return &_mutex; }
WALLE_NON_COPYABLE(TimedMutex);
private:
    NativeType   _mutex;  
};

#else
class TimedMutex {
public:
    typedef Mutex::NativeType NativeType;
    typedef NativeType*       NativeTypePtr;
public:
    TimedMutex() noexcept
    : _locked(false)
    {

    }
    ~TimedMutex();

    void lock();
    bool try_lock();
    void unlock();
    bool try_lock_for(Timespan span);
    bool try_lock_until(Timestamp stamp);
    NativeTypePtr native_type_ptr();
WALLE_NON_COPYABLE(TimedMutex);
private:
    Mutex       _mutex;
    Condition   _cond;
    bool        _locked;
};
#endif

} //namespace walle
#endif //WALLE_SYS_MUTEX_H_