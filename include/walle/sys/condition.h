#ifndef WALLE_SYS_CONDITION_H_
#define WALLE_SYS_CONDITION_H_
#include <walle/sys/detail/mutex_base.h>
#include <pthread.h>

namespace walle {

class Condition {
public:
    typedef pthread_cond_t CondType;
    typedef CondType*      CondTypePtr;
public:
    Condition():_cond(PTHREAD_COND_INITIALIZER)
    {
    }
    ~Condition() { }
    WALLE_NON_COPYABLE(Condition);
public:
    
    void signal();

    void broadcast();

    void wait(UniqueLock<Mutex> &mutex);

    void wait(Mutex &mutex);

    bool wait_for(UniqueLock<Mutex> &mutex, Timespan span);

    bool wait_for(Mutex &mutex, Timespan span);

    bool wait_until(UniqueLock<Mutex> &mutex, Timestamp stamp);

    bool wait_until(Mutex &mutex, Timestamp stamp);
private:
    CondType   _cond;
};

} //namespace walle

#endif //WALLE_SYS_CONDITION_H_