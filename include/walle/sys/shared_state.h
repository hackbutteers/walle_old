#ifndef WALLE_SYS_SHARED_STATE_H_
#define WALLE_SYS_SHARED_STATE_H_
#include <walle/sys/condition.h>
#include <walle/sys/mutex.h>
#include <walle/time/timespan.h>
#include <walle/config/base.h>
#include <cstddef>

namespace walle {

template <typename T = size_t>

class SharedState {
public:
	typedef T value_type;
public:
	
	SharedState(const T &value):_value(value), _mutex(), _cond()
	{
	}

	void set_to(const T&value)
	{
		UniqueLock<Mutex> lock(_mutex);
		_value = value;
		_cond.broadcast();
	}

	void wait_for(const T &condition)
	{
		UniqueLock<Mutex> lock(_mutex);
		if(_value != condition) {
			_cond.wait(lock);
		}
	}
	
	bool wait_for(const T &condition, Timespan span)
	{	if(span.total_microseconds() == 0) {
			return false;
		}
		UniqueLock<Mutex> lock(_mutex);
		if(_value != condition) {
			_cond.wait_for(lock, span);
		}
		return true;
	}

	value_type get() 
	{
		UniqueLock<Mutex> lock(_mutex);
		return _value;
	}
private:
	value_type _value;
	Mutex      _mutex;
	Condition  _cond;

};
} //namespace walle

#endif