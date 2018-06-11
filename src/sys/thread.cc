#include <walle/sys/thread.h>
#include <walle/sys/this_thread.h>
#include <walle/format/format.h>
#include <walle/sys/env.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <cstring>
#include <pthread.h>
#if WALLE_PLATEFORM == WALLE_PLATEFORM_OSX
    #include <mach/mach.h>
	#include <mach/task.h>
	#include <mach/thread_policy.h>
#endif
namespace walle {

namespace detail {
	
	__thread walle::Thread *current_thread_ptr = NULL;
}

	class Thread::ThreadData {
	public:
		ThreadData():
			thread(0),
			prio(Thread::PRIO_NORMAL),
			policy(Thread::POLICY_DEFAULT),
			event(Thread::RS_STOP),
			stackSize(),
			started(false),
			joined(false)
		{

		}

		std::shared_ptr<Runnable>     pRunnableTarget;
		pthread_t                     thread;
		walle::this_thread::tid_t     tid;
		int                           prio;
		int                           osPrio;
		int                           policy;
		SharedState<int>              event;
		std::size_t                   stackSize;
		bool                          started;
		bool                          joined;
	};


	class FunctorRunnable: public Runnable {
	public:
		FunctorRunnable(const Thread::ThreadFactor& functor):
			_functor(functor)
		{
		}

		~FunctorRunnable()
		{
		}

		void run()
		{
			_functor();
		}

	private:
		Thread::ThreadFactor  _functor;
	};


Thread::Thread(): 
	_id(unique_id()),
	_threadData(new ThreadData()), 
	_name(make_name()),
	_sleepState(SS_WAKE),
	_mutex() 
{
}


Thread::Thread(const std::string& rName): 
	_id(unique_id()),
	_threadData(new ThreadData()), 
	_name(rName+make_name()),
	_sleepState(SS_WAKE),
	_mutex() 
{
}


Thread::~Thread()
{
	if(!_threadData->started && !_threadData->joined ) {
		::pthread_detach(_threadData->thread);
	}
}


bool Thread::set_priority(Priority prio)
{
	if(prio != _threadData->prio) {
		_threadData->prio = prio;
		if(is_running()) {
			struct sched_param par;
			struct MyStruct
			{

			};
			par.sched_priority = map_priority(_threadData->prio, SCHED_OTHER);
			if (pthread_setschedparam(_threadData->thread, SCHED_OTHER, &par)) {
				return false;
			}
		}
	}
	return true;
}


Thread::Priority Thread::get_priority() const
{
	return static_cast<Thread::Priority>(_threadData->prio);
}

bool Thread::set_os_priority(int prio, int policy)
{
	if (prio != _threadData->osPrio || policy != _threadData->policy) {
		if (_threadData->pRunnableTarget) {
			struct sched_param par;
			par.sched_priority = prio;
			if (pthread_setschedparam(_threadData->thread, policy, &par)) {
				return false;
			}
				
		}
		_threadData->prio   = map_priority_reverse(prio, policy);
		_threadData->osPrio = prio;
		_threadData->policy = policy;
	}
	return true;
}

int Thread::get_os_priority() const
{
	return _threadData->osPrio;
}

bool Thread::start(const RunnablePtr& target)
{
	//can not be null should abort on lauch.
	assert(target);
	//can not be start twice should abort on lauch.
	assert(target != _threadData->pRunnableTarget);
	//can not be start after join should abort on lauch.
	assert(!_threadData->joined);
	_threadData->pRunnableTarget = target;
	
	pthread_attr_t attributes;
	pthread_attr_init(&attributes);

	if (_threadData->stackSize != 0) {
		if (0 != pthread_attr_setstacksize(&attributes, _threadData->stackSize)) {
			pthread_attr_destroy(&attributes);
			_threadData->pRunnableTarget.reset();
			return false;
		}
	}
	if (pthread_create(&_threadData->thread, &attributes, thread_fuction, this)) {
		_threadData->pRunnableTarget.reset();
		pthread_attr_destroy(&attributes);
		return false;
	}
	_threadData->started = true;
	pthread_attr_destroy(&attributes);
	if (_threadData->prio != PRIO_NORMAL) {
		struct sched_param par;
		par.sched_priority = map_priority(_threadData->prio, SCHED_OTHER);
		::pthread_setschedparam(_threadData->thread, SCHED_OTHER, &par);
	}
	_threadData->event.wait_for(RS_RUNING);
	return true;

}


bool Thread::start(const ThreadFactor &fn)	
{
	RunnablePtr ptr(new FunctorRunnable(fn));
	return start(ptr);
}

Thread* Thread::current()
{
 	return detail::current_thread_ptr;
}
void Thread::join()
{
	if(!_threadData){
		return ;
	}

	if(!_threadData->started || _threadData->joined) {
		return ;
	}
	_threadData->event.wait_for(RS_STOP);
	void *result = NULL;
	pthread_join(_threadData->thread, &result);
	_threadData->joined = true;
}

bool Thread::try_sleep(long milliseconds)
{
	Thread* pT = Thread::current();
	if(!pT) {
		return false;
	}
	pT->_sleepState.set_to(SS_SLEEP);
	return (pT->_sleepState.wait_for(SS_WAKE, milliseconds));
}


void Thread::wake_up()
{
	_sleepState.set_to(SS_WAKE);
}


std::string Thread::make_name()
{
	MemoryWriter out;
	out.write("#{}", _id);
	return out.str();
}

bool Thread::is_running() const
{
	if(_threadData->pRunnableTarget ){
		return true;
	} 
	return false;
}

int Thread::unique_id()
{
	static std::atomic<int32_t> counter;
	return ++counter;
}

bool Thread::set_affinity(int cpu)
{
#if WALLE_PLATEFORM == WALLE_PLATEFORM_LINUX
    cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);
	if (::pthread_setaffinity_np(_threadData->thread, sizeof(cpuset), &cpuset) != 0) {
		return false;
    }
#elif WALLE_PLATEFORM == WALLE_PLATEFORM_OSX
    kern_return_t ret;
	thread_affinity_policy policy;
	policy.affinity_tag = cpu;

	ret = thread_policy_set(pthread_mach_thread_np(_threadData->thread),
		THREAD_AFFINITY_POLICY,
		(thread_policy_t)&policy,
		THREAD_AFFINITY_POLICY_COUNT);
	if (ret != KERN_SUCCESS) {
		return false;
    }
#else
#error "no impl"
#endif
	walle::this_thread::yield();
    return true;
    
}

int Thread::get_affinity() const
{
    int cpuSet = -1;
#if WALLE_PLATEFORM == WALLE_PLATEFORM_LINUX
	
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	if (pthread_getaffinity_np(_threadData->thread, sizeof(cpuset), &cpuset) != 0) {
		return false;
	}
	int cpuCount = Environment::processor_count();
	for (int i = 0; i < cpuCount; i++) {
		if (CPU_ISSET(i, &cpuset)) {
			cpuSet = i;
			break;
		}
    }
#elif WALLE_PLATEFORM == WALLE_PLATEFORM_OSX
    kern_return_t ret;
	thread_affinity_policy policy;
	mach_msg_type_number_t count = THREAD_AFFINITY_POLICY_COUNT;
	boolean_t get_default = false;
	ret = thread_policy_get(pthread_mach_thread_np(_threadData->thread),
		THREAD_AFFINITY_POLICY,
		(thread_policy_t)&policy,
		&count,
		&get_default);
	if (ret != KERN_SUCCESS) {
		return false;
	}
	cpuSet = policy.affinity_tag;
	int cpuCount = Environment::processor_count();
	if (cpuSet >= cpuCount)
		cpuSet = -1;
#else
#error "no impl"
#endif
	return cpuSet;
}

int Thread::id() const
{
	return _id;
}

Thread::TID Thread::tid() const
{
	return _threadData->tid;
}
void Thread::set_name(const std::string& rName)
{
	UniqueLock<Mutex> lock(_mutex);
	_name = rName;
}

std::string Thread::name() const
{
	UniqueLock<Mutex> lock(_mutex);
	return _name;
}

std::string Thread::get_name() const
{
	UniqueLock<Mutex> lock(_mutex);
	return _name;
}
int Thread::get_min_os_priority(int policy)
{
	return sched_get_priority_min(policy);
}

void* Thread::thread_fuction(void *data)
{
	Thread* pthis = static_cast<Thread*>(data);
	pthis->_threadData->event.set_to(RS_RUNING);
	detail::current_thread_ptr = pthis;
	pthis->_threadData->tid = walle::this_thread::tid();
	walle::this_thread::set_thread_name(pthis->_threadData->thread, pthis->get_name());
	pthis->_threadData->pRunnableTarget->run();
	pthis->_threadData->event.set_to(RS_STOPING);
	pthis->_threadData->pRunnableTarget.reset();
	pthis->_threadData->started = false;
	pthis->_threadData->event.set_to(RS_STOP);
	return NULL;
}

int Thread::get_max_os_priority(int policy)
{
	return sched_get_priority_max(policy);
}

int Thread::map_priority(int prio, int plicy)
{
	int pmin = get_min_os_priority(plicy);
	int pmax = get_max_os_priority(plicy);

	switch (prio) {
	case PRIO_LOWEST:
		return pmin;
	case PRIO_LOW:
		return pmin + (pmax - pmin) / 4;
	case PRIO_NORMAL:
		return pmin + (pmax - pmin) / 2;
	case PRIO_HIGH:
		return pmin + 3 * (pmax - pmin) / 4;
	case PRIO_HIGHEST:
		return pmax;
	default:
		return pmin;
	}
	return pmin;
}

int Thread::map_priority_reverse(int prio, int plicy)
{
	if (plicy == SCHED_OTHER) {
		int pmin = get_min_os_priority(plicy);
		int pmax = get_max_os_priority(plicy);
		int normal = pmin + (pmax - pmin) / 2;
		if (prio == pmax)
			return PRIO_HIGHEST;
		if (prio > normal)
			return PRIO_HIGH;
		else if (prio == normal)
			return PRIO_NORMAL;
		else if (prio > pmin)
			return PRIO_LOW;
		else
			return PRIO_LOWEST;
	} 
	
	return PRIO_HIGHEST;
}

} //namespace walle 
