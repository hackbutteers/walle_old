#include <walle/sys/this_thread.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

namespace walle {
namespace this_thread{
__thread tid_t     t_tid = 0;
__thread int       t_is_main = -1;  

  
void set_thread_name(pthread_t thread, const std::string &threadName)
{
#if WALLE_PLATEFORM == WALLE_PLATEFORM_LINUX
    if (::pthread_setname_np(thread, threadName.c_str()) == ERANGE && threadName.size() > 15) {
        std::string truncName(threadName, 0, 7);
        truncName.append("~");
        truncName.append(threadName, threadName.size() - 7, 7);
        pthread_setname_np(thread, truncName.c_str());
    }
#elif WALLE_PLATEFORM == WALLE_PLATEFORM_OSX
    pthread_setname_np(threadName.c_str());
#else

#endif
}

std::string get_thread_name(pthread_t thread)
{
#if WALLE_PLATEFORM == WALLE_PLATEFORM_LINUX
    static const int kMaxThreadNameLen = 128;
    char buffer[kMaxThreadNameLen]={};
    pthread_getname_np(thread, buffer, kMaxThreadNameLen - 1);
    return std::string(buffer);
#else
    return "";
#endif
}
	void cache_tid()
	{
#if WALLE_PLATEFORM == WALLE_PLATEFORM_LINUX
	  if (t_tid == 0) {
	    	t_tid = static_cast<size_t>(syscall(SYS_gettid));
	  }
#elif WALLE_PLATEFORM == WALLE_PLATEFORM_OSX
	if(t_tid == 0) {
		uint64_t tid;
    	pthread_threadid_np(WALLE_NULL, &tid);
     	t_tid = static_cast<size_t>(tid);
	}
#endif
    }
    
    void cache_main()
	{
	  if (t_is_main == -1) {
#if WALLE_PLATEFORM == WALLE_PLATEFORM_LINUX
		t_is_main = static_cast<size_t>(syscall(SYS_getpid)) == tid() ? 1 : 0;
#elif WALLE_PLATEFORM == WALLE_PLATEFORM_OSX
	    	t_is_main = pthread_main_np();
#endif
	  }
	}
	bool is_main_thread()
	{
        if(t_is_main == -1) {
            cache_main();
        }
        return t_is_main == 1 ;
	}
		
	void exit()
	{
		::pthread_exit(NULL);
	}
		
	void yield()
	{
		::sched_yield();
	}

	bool sleep_for(walle::Timespan span)
	{

		struct timespec ts = { 0, 0 };
		ts.tv_sec = static_cast<time_t>(span.total_seconds());
		ts.tv_nsec = static_cast<long>(span.microseconds() * 1000);
		::nanosleep(&ts, NULL);

	return true;

	}

	bool sleep_until(walle::Timestamp stamp)
	{
		walle::Timestamp now;
		if(now >= stamp) {
			return false;
		} 
		walle::Timespan span = now -stamp;
		return  sleep_for(span);
	}

} //namespace this_thread


} //namespace walle
