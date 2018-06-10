#include <walle/sys/condition.h>

namespace walle {

void Condition::signal()
{
    ::pthread_cond_signal(&_cond);
}

void Condition::broadcast()
{
    ::pthread_cond_broadcast(&_cond);
}

void Condition::wait(UniqueLock<Mutex> &mutex)
{
    ::pthread_cond_wait(&_cond, mutex.mutex()->native_type_ptr());
}

void Condition::wait(Mutex &mutex)
{
    ::pthread_cond_wait(&_cond, mutex.native_type_ptr());
}

bool Condition::wait_for(UniqueLock<Mutex> &mutex, Timespan span)
{
    Timestamp now;
    now += span.total_microseconds();
    struct timespec ts = now;
    int32_t ecode = ::pthread_cond_timedwait(&_cond, mutex.mutex()->native_type_ptr(), &ts);
    if (ecode == 0) 
        return true;
	return false;
}

bool Condition::wait_for(Mutex &mutex, Timespan span)
{
    Timestamp now;
    now += span.total_microseconds();
    struct timespec ts = now;
    int32_t ecode = ::pthread_cond_timedwait(&_cond, mutex.native_type_ptr(), &ts);
    if (ecode == 0) 
        return true;
	return false;
}

bool Condition::wait_until(UniqueLock<Mutex> &mutex, Timestamp stamp)
{
    struct timespec ts = stamp;
    int32_t ecode = ::pthread_cond_timedwait(&_cond,  mutex.mutex()->native_type_ptr(), &ts);
    if (ecode == 0) 
        return true;
	return false;
}

bool Condition::wait_until(Mutex &mutex, Timestamp stamp)
{
    struct timespec ts = stamp;
    int32_t ecode = ::pthread_cond_timedwait(&_cond,  mutex.native_type_ptr(), &ts);
    if (ecode == 0) 
        return true;
	return false;
}

} //namespace walle