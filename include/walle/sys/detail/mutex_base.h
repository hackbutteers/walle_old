#ifndef WALLE_SYS_DETAIL_MUTEX_BASE_H_
#define WALLE_SYS_DETAIL_MUTEX_BASE_H_
#include <walle/time/timespan.h>
#include <walle/time/timestamp.h>
#include <algorithm>
#include <pthread.h>

namespace walle {

class Mutex {
public:
    typedef pthread_mutex_t NativeType;
    typedef NativeType*     NativeTypePtr;
public:
    Mutex() noexcept
    : _mutex(PTHREAD_MUTEX_INITIALIZER)
    {

    }
    ~Mutex() {}

    void lock();
    bool try_lock();
    void unlock();
    NativeTypePtr native_type_ptr() { return &_mutex; }
    WALLE_NON_COPYABLE(Mutex);
private:
    NativeType   _mutex;
};

class NullMutex{
public:
    typedef pthread_mutex_t NativeType;
    typedef NativeType*     NativeTypePtr;
public:
    NullMutex() {}
    ~NullMutex() {}
    void lock() {}
    bool try_lock() { return true; }
    void unlock() {}  
    NativeTypePtr native_type_ptr(){ return nullptr; }   
};

template <typename _Mutex>
class LockGard{
public:
    typedef _Mutex MutexType;
    explicit LockGard(const MutexType &m)
        :_mutex(m)
    {
        _mutex.lock();
    }

    ~LockGard()
    {
        _mutex.unlock();
    }
    
    WALLE_NON_COPYABLE(LockGard);
private:
    MutexType  _mutex;
};

struct DeferLock{};
struct TryLock{};
struct AdoptLock{};

constexpr DeferLock deferlock = DeferLock();
constexpr TryLock   trylock = TryLock();
constexpr AdoptLock adoptlock = AdoptLock();

template <typename _Mutex>
class UniqueLock{
public:
    typedef _Mutex     MutexType;
    typedef MutexType* MutexTypePtr;
    typedef MutexType& MutexTypeRef;
public:
    UniqueLock():_mutex(nullptr), _own(false) {}

    /** 
     * @brief  normal RATII
     * @note   
     * @param  &m: 
     * @retval None
     */
    explicit UniqueLock(MutexType &m)
        :_mutex(&m),
         _own(true)
    {
        _mutex->lock();
    }

    /** 
     * @brief  do nothing
     * @note   
     * @param  &m: 
     * @param  &d: 
     * @retval 
     */
    UniqueLock(MutexType &m, DeferLock &d) noexcept
        :_mutex(&m),
         _own(false)
    {
        WALLE_UNUSED(d);
    }

    /** 
     * @brief  try lock the lock
     * @note   
     * @param  &m: 
     * @param  &d: 
     * @retval 
     */
    UniqueLock(MutexType &m, const TryLock &d)
        :_mutex(&m),
         _own(_mutex->try_lock())
    {

    }

    /** 
     * @brief  already lock, just for defer
     * @note   
     * @param  &m: 
     * @param  &d: 
     * @retval 
     */
    UniqueLock(MutexType &m, AdoptLock &d) noexcept
        :_mutex(&m),
         _own(true)
    {
    }

    /** 
     * @brief  lock until the time outs
     * @note   
     * @param  &m: 
     * @param  deadline: 
     * @retval 
     */
    UniqueLock(MutexType &m, Timestamp deadline)
        :_mutex(&m),
         _own(_mutex->try_lock_until(deadline))
    {

    }

    /** 
     * @brief  try lock for some timespan
     * @note   
     * @param  &m: 
     * @param  duration: 
     * @retval 
     */
    UniqueLock(MutexType &m, Timespan duration)
        :_mutex(&m),
         _own(_mutex->try_lock_for(duration))
    {

    }
    /** 
     * @brief  move the other one
     * @note   
     * @param  &&rhs: 
     * @retval 
     */
    UniqueLock(UniqueLock &&rhs)
        :_mutex(rhs._mutex),
         _own(rhs._own)
    {
        rhs._mutex = nullptr;
        rhs._own = false;
    }
    /** 
     * @brief  move operator
     * @note   
     * @param  other: 
     * @retval None
     */
    UniqueLock& operator = (UniqueLock && other)
    {
        if (_own) {
            _mutex->unlock();
        }
        _mutex = other._mutex;
        _own = other._own;
        other._mutex = nullptr;
        other._own = false;
        return *this;
    }
    
    /** 
     * @brief  destroy the lock
     * @note   
     * @retval 
     */
    ~UniqueLock()
    {
        if(_own) {
            _mutex->unlock();
        }
    }

    WALLE_NON_COPYABLE(UniqueLock);
public:

    /** 
     * @brief  lock manully
     * @note   
     * @retval None
     */
    void lock();

    /** 
     * @brief  try a lock
     * @note   
     * @retval 
     */
    bool try_lock();

    /** 
     * @brief  try lock for some time.
     * @note   
     * @param  span: 
     * @retval 
     */
    bool try_lock_for(Timespan span);

    /** 
     * @brief  try lock until some time.
     * @note   
     * @param  stamp: 
     * @retval 
     */
    bool try_lock_until(Timestamp stamp);

    /** 
     * @brief  release the lock
     * @note   
     * @retval None
     */
    void unlock();

    /** 
     * @brief  do not hold the lock any more.
     * @note   
     * @retval 
     */
    MutexTypePtr release()
    {
        MutexTypePtr ptr = _mutex;
        _mutex = nullptr;
        _own = false;
        return ptr;
    }

    /** 
     * @brief  true if locked
     * @note   
     * @retval 
     */
    bool owns_lock() const
    {
        return _own;
    }
    operator bool() const
    {
        return _own;
    }

    /** 
     * @brief  lock type pointer
     * @note   
     * @retval 
     */
    MutexTypePtr mutex()
    {
        return _mutex;
    }

    /** 
     * @brief  swap the two lock
     * @note   
     * @param  &rhs: 
     * @retval None
     */
    void swap(UniqueLock &rhs)
    {
        std::swap(_mutex, rhs._mutex);
        std::swap(_own, rhs._own);
    }
private:
    MutexTypePtr   _mutex;
    bool           _own;
};

template <typename _Mutex>
void UniqueLock<_Mutex>::lock()
{
    WALLE_ASSERT(_mutex,"_mutex is nullptr");
    WALLE_ASSERT(!_own, "has been locked");
    _mutex->lock();
    _own = true;
}

template <typename _Mutex>
bool UniqueLock<_Mutex>::try_lock()
{
    WALLE_ASSERT(_mutex,"_mutex is nullptr");
    WALLE_ASSERT(!_own, "has been locked");
    _own = _mutex->try_lock();
    return _own;
}

template <typename _Mutex>
bool UniqueLock<_Mutex>::try_lock_for(Timespan span)
{
    WALLE_ASSERT(_mutex,"_mutex is nullptr");
    WALLE_ASSERT(!_own, "has been locked");
    _own = _mutex->try_lock_for(span);
    return _own;
}

template <typename _Mutex>
bool UniqueLock<_Mutex>::try_lock_until(Timestamp stamp)
{
    WALLE_ASSERT(_mutex,"_mutex is nullptr");
    WALLE_ASSERT(!_own, "has been locked");
    _own = _mutex->try_lock_until(stamp);
    return _own;
}

template <typename _Mutex>
void UniqueLock<_Mutex>::unlock()
{
    WALLE_ASSERT(_own, " should be locked");
    _mutex->unlock();
    _own = false;
}

/** 
 * @brief  global swap 
 * @note   
 * @param  &u1: 
 * @param  &u2: 
 * @retval None
 */
template <typename _Mutex>
WALLE_FORCE_INLINE void swap(UniqueLock<_Mutex> &u1, UniqueLock<_Mutex> &u2)
{
    u1.swap(u2);
}
} //namespace walle
#endif //WALLE_SYS_DETAIL_MUTEX_BASE_H_