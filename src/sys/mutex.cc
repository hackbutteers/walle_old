#include <walle/sys/mutex.h>
#include <iostream>
namespace walle {

void Mutex::lock()
{
    int ret;
    if ((ret = ::pthread_mutex_lock(&_mutex)) != 0) {
            std::cout<<ret<<std::endl;
	  		WALLE_ASSERT(false, "lock fail");
	}
}

bool Mutex::try_lock()
{
    int32_t ecode = ::pthread_mutex_trylock(&_mutex);
    if (ecode == 0) {
        return true;
    }
	return false;
}

void Mutex::unlock()
{
    ::pthread_mutex_unlock(&_mutex);
}
#if WALLE_PLATEFORM == WALLE_PLATEFORM_LINUX

void TimedMutex::lock()
{
    if (::pthread_mutex_lock(&_mutex) != 0) {
	  		WALLE_ASSERT(false, "lock fail");
	}
}

bool TimedMutex::try_lock()
{
    int32_t ecode = ::pthread_mutex_trylock(&_mutex);
    if (ecode == 0) {
        return true;
    }
	return false;
}

void TimedMutex::unlock()
{
    ::pthread_mutex_unlock(&_mutex);
}

bool TimedMutex::try_lock_for(Timespan span)
{
    Timestamp now;
    now += span.microseconds();
    struct ::timespec ts = now;
    int32_t ecode = ::pthread_mutex_timedlock(&_mutex, &ts);
    if (ecode == 0) 
        return true;
	return false;

}
bool TimedMutex::try_lock_until(Timestamp stamp)
{
    struct ::timespec ts = stamp;
    int32_t ecode = ::pthread_mutex_timedlock(&_mutex, &ts);
    if (ecode == 0) 
        return true;
	return false;
}
#else
void TimedMutex::lock()
{
    WALLE_ASSERT(!_locked, "already be locked");
    UniqueLock<Mutex> gard(_mutex);
    while(_locked) 
        _cond.wait(_mutex);
    _locked = true;
}

bool TimedMutex::try_lock()
{
    WALLE_ASSERT(!_locked, "already be locked");
    UniqueLock<Mutex> gard(_mutex, trylock);
    _locked = gard.owns_lock();
    return _locked;

}

void TimedMutex::unlock()
{
    WALLE_ASSERT(_locked, "should be locked");
    UniqueLock<Mutex> gard(_mutex);
    _locked = false;
    _cond.signal();
}

bool TimedMutex::try_lock_for(Timespan span)
{
    WALLE_ASSERT(!_locked, "already be locked");
    UniqueLock<Mutex> gard(_mutex);
    _locked = _cond.wait_for(gard, span);
    return _locked;
}
bool TimedMutex::try_lock_until(Timestamp stamp)
{
    WALLE_ASSERT(!_locked, "already be locked");
    UniqueLock<Mutex> gard(_mutex);
    _locked = _cond.wait_until(gard, stamp);
    return _locked;
}

TimedMutex::~TimedMutex()
{
     UniqueLock<Mutex> gard(_mutex);    
}
#endif
} //namespace walle