#ifndef WALLE_SYS_THIS_THREAD_H_
#define WALLE_SYS_THIS_THREAD_H_

#include <walle/time/timestamp.h>
#include <walle/time/timespan.h>
#include <walle/config/base.h>
#include <string>
#include <pthread.h>

namespace walle {
namespace this_thread {

extern __thread size_t    t_tid;
extern __thread int       t_is_main;
void cache_tid();

typedef size_t tid_t;

WALLE_FORCE_INLINE tid_t tid()
{
    if (WALLE_UNLIKELY(t_tid == 0)) {
        cache_tid();
    }
    return t_tid;
}

void set_thread_name(pthread_t thread, const std::string &threadName);

void get_thread_name();
/** 
 * @brief return true if the thread is main thread pid = thread_id
 * @note   
 * @retval 
 */
bool is_main_thread();

/** 
 * @brief Suspends the current thread for the specified amount of time.
 * @note   
 * @param  span: 
 * @retval 
 */
bool sleep_for(walle::Timespan span);

/** 
 * @brief Suspends the current thread for the specified 
 *        amount of time, a another way to sleep for sleep.
 * @note   
 * @param  stamp: 
 * @retval 
 */
bool sleep_until(walle::Timestamp stamp);

/** 
 * @brief Yields cpu to other threads.
 * @note   
 * @retval None
 */
void yield();

/** 
 * @brief exit current thread. 
 * @note   
 * @retval None
 */
void exit();

} //namespace this_thread

} //namespace walle
#endif
