#ifndef WALLE_COMMON_DELEGATE_H_
#define WALLE_COMMON_DELEGATE_H_
#include <walle/config/base.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

/** 
 * @brief  This is a faster replacement than std::function.
 * @note   idea from http://codereview.stackexchange.com/questions/14730/impossibly-fast-delegate
 * 
 * @retval None
 */
namespace wsl {
template <typename T, typename Allocator = std::allocator<void> >
class basic_delegate;

template <typename R, typename... A, typename Allocator>
class basic_delegate<R(A...), Allocator> {
public:
    /** 
     * @brief  default constructor
     * @note   
     * @retval 
     */
    basic_delegate() = default;

    /** 
     * @brief copy constructor  
     * @note   
     * @param  basic_delegate&: 
     * @retval 
     */
    basic_delegate(const basic_delegate&) = default;

    /** 
     * @brief move constructor
     * @note   
     * @retval 
     */
    basic_delegate(basic_delegate&&) = default;

    /** 
     * @brief copy assignment operator
     * @note   
     * @param  basic_delegate&: 
     * @retval None
     */
    basic_delegate& operator = (const basic_delegate&) = default;

    /** 
     * @brief move assignment operator 
     * @note   
     * @retval None
     */
    basic_delegate& operator = (basic_delegate&&) = default;

    /** 
     * @brief  construction from an immediate function with no object or pointer.
     * @note   
     * @param  Function: 
     * @retval 
     */
    template <R(* const Function)(A...)>
    static basic_delegate make() WALLE_NOEXCEPT 
    {
        return basic_delegate(function_caller<Function>, nullptr);
    }

    /** 
     * @brief constructor from a plain function pointer with no object. 
     * @note   
     * @param  function_ptr: 
     * @retval None
     */
    explicit basic_delegate(R (*const function_ptr)(A...)) WALLE_NOEXCEPT
        : basic_delegate(function_ptr_caller,
                   * reinterpret_cast<void* const*>(&function_ptr))
    {

    }

    WALLE_STATIC_ASSERT(sizeof(void*) == sizeof(void (*)(void)),
                  "object pointer and function pointer sizes must equal");

    /** 
     * @brief construction from a plain function pointer with no object. 
     * @note   
     * @param  function_ptr: 
     * @retval 
     */
    static basic_delegate make(R (*const function_ptr)(A...)) WALLE_NOEXCEPT 
    {
        return basic_delegate(function_ptr);
    }

    /** 
     * @brief  construction for an immediate class::method with class object 
     * @note   
     * @param  Method: 
     * @retval 
     */
    template <class C, R(C::* const Method)(A...)>
    static basic_delegate make(C* const object_ptr) WALLE_NOEXCEPT {
        return basic_delegate(method_caller<C, Method>, object_ptr);
    }

    /** 
     * @brief construction for an immediate class::method with class object
     * @note   
     * @param  Method: 
     * @retval 
     */
    template <class C, R(C::* const Method)(A...) const>
    static basic_delegate make(C const* const object_ptr) WALLE_NOEXCEPT {
        return basic_delegate(const_method_caller<C, Method>,
                        const_cast<C*>(object_ptr));
    }

    /** 
     * @brief construction for an immediate class::method with class object by reference.
     * @note   
     * @param  Method: 
     * @retval 
     */
    template <class C, R(C::* const Method)(A...)>
    static basic_delegate make(C& object) WALLE_NOEXCEPT 
    {
        return basic_delegate(method_caller<C, Method>, &object);
    }

    /** 
     * @brief construction for an immediate class::method with class object by reference 
     * @note   
     * @param  Method: 
     * @retval 
     */
    template <class C, R(C::* const Method)(A...) const>
    static basic_delegate make(C const& object) WALLE_NOEXCEPT 
    {
        return basic_delegate(const_method_caller<C, Method>,
                        const_cast<C*>(&object));
    }
    
    /** 
     * @brief constructor from any functor object T, 
     *        which may be a lambda with capture or a MemberPair or
     *        ConstMemberPair wrapper. 
     * @note   
     * @param  f: 
     * @retval 
     */
    template <
        typename T,
        typename = typename std::enable_if<
            !std::is_same<basic_delegate, typename std::decay<T>::type>::value
            >::type
        >
    basic_delegate(T&& f)
        : _store(
              // allocate memory for T in shared_ptr with appropriate deleter
              typename Allocator::template rebind<
                  typename std::decay<T>::type>::other().allocate(1),
              store_deleter<typename std::decay<T>::type>, Allocator()) 
    {

        using Functor = typename std::decay<T>::type;
        using Rebind = typename Allocator::template rebind<Functor>::other;

        // copy-construct T into shared_ptr memory.
        Rebind().construct(
            static_cast<Functor*>(_store.get()), Functor(std::forward<T>(f)));

        _object_ptr = _store.get();

        _caller = functor_caller<Functor>;
    }

    /** 
     * @brief constructor from any functor object T, which may be a lambda with 
     *        capture or a MemberPair or ConstMemberPair wrapper.  
     * @note   
     * @param  f: 
     * @retval 
     */
    template <typename T>
    static basic_delegate make(T&& f) {
        return std::forward<T>(f);
    }

    /** 
     * @brief constructor for wrapping a class::method with object pointer. 
     * @note   
     * @param  object_ptr: 
     * @param  method_ptr: 
     * @retval 
     */
    template <class C>
    basic_delegate(C* const object_ptr, R (C::* const method_ptr)(A...))
        : basic_delegate(MemberPair<C>(object_ptr, method_ptr)) 
    { 

    }

    /** 
     * @brief constructor for wrapping a const class::method with object pointer.
     * @note   
     * @param  object_ptr: 
     * @param  method_ptr: 
     * @retval 
     */
    template <class C>
    basic_delegate(C* const object_ptr, R (C::* const method_ptr)(A...) const)
        : basic_delegate(ConstMemberPair<C>(object_ptr, method_ptr)) 
    { 

    }

    /** 
     * @brief constructor for wrapping a class::method with object reference.
     * @note   
     * @param  object: 
     * @param  method_ptr: 
     * @retval 
     */
    template <class C>
    basic_delegate(C& object, R (C::* const method_ptr)(A...))
        : basic_delegate(MemberPair<C>(&object, method_ptr)) { }

    /** 
     * @brief constructor for wrapping a const class::method with object reference. 
     * @note   
     * @param  object: 
     * @param  method_ptr: 
     * @retval 
     */
    template <class C>
    basic_delegate(C const& object, R (C::* const method_ptr)(A...) const)
        : basic_delegate(ConstMemberPair<C>(&object, method_ptr)) { }

    /** 
     * @brief constructor for wrapping a class::method with object pointer. 
     * @note   
     * @param  object_ptr: 
     * @param  method_ptr: 
     * @retval 
     */
    template <class C>
    static basic_delegate make(C* const object_ptr,
                         R (C::* const method_ptr)(A...)) {
        return MemberPair<C>(object_ptr, method_ptr);
    }

    /** 
     * @brief constructor for wrapping a const class::method with object pointer. 
     * @note   
     * @param  object_ptr: 
     * @param  method_ptr: 
     * @retval 
     */
    template <class C>
    static basic_delegate make(C const* const object_ptr,
                         R (C::* const method_ptr)(A...) const) {
        return ConstMemberPair<C>(object_ptr, method_ptr);
    }

    /** 
     * @brief constructor for wrapping a class::method with object reference.
     * @note   
     * @param  object: 
     * @param  method_ptr: 
     * @retval 
     */
    template <class C>
    static basic_delegate make(C& object, R (C::* const method_ptr)(A...)) {
        return MemberPair<C>(&object, method_ptr);
    }

    /** 
     * @brief constructor for wrapping a const class::method with object reference.
     * @note   
     * @param  object: 
     * @param  method_ptr: 
     * @retval 
     */
    template <class C>
    static basic_delegate make(C const& object,
                         R (C::* const method_ptr)(A...) const) {
        return ConstMemberPair<C>(&object, method_ptr);
    }

    /** 
     * @brief  reset delegate to invalid.
     * @note   
     * @retval None
     */
    void reset() { _caller = nullptr; _store.reset(); }

    void reset_caller() WALLE_NOEXCEPT { _caller = nullptr; }

    /** 
     * @brief swap delegates 
     * @note   
     * @param  other: 
     * @retval None
     */
    void swap(basic_delegate& other) WALLE_NOEXCEPT 
    { 
        std::swap(*this, other); 
    }

    /** 
     * @brief compare delegate with another
     * @note   
     * @param  rhs: 
     * @retval None
     */
    bool operator == (basic_delegate const& rhs) const WALLE_NOEXCEPT 
    {
        return (_object_ptr == rhs._object_ptr) && (_caller == rhs._caller);
    }

    /** 
     * @brief compare delegate with another. 
     * @note   
     * @param  rhs: 
     * @retval None
     */
    bool operator != (basic_delegate const& rhs) const WALLE_NOEXCEPT 
    {
        return !operator == (rhs);
    }

    /** 
     * @brief compare delegate with another
     * @note   
     * @param  rhs: 
     * @retval None
     */
    bool operator < (basic_delegate const& rhs) const WALLE_NOEXCEPT 
    {
        return (_object_ptr < rhs._object_ptr) ||
               ((_object_ptr == rhs._object_ptr) && (_caller < rhs._caller));
    }

    /** 
     * @brief compare delegate with another
     * @note   
     * @param  const: 
     * @retval None
     */
    bool operator == (std::nullptr_t const) const WALLE_NOEXCEPT 
    {
        return _caller == nullptr;
    }

    //! compare delegate with another
    bool operator != (std::nullptr_t const) const WALLE_NOEXCEPT 
    {
        return _caller != nullptr;
    }

    /** 
     * @brief  explicit conversion to bool -> valid or invalid. 
     * @note   
     * @retval None
     */
    explicit operator bool () const WALLE_NOEXCEPT 
    { 
        return _caller != nullptr; 
    }

    /** 
     * @brief most important method: call. The call is forwarded to the 
     *        selected function caller.
     * @note   
     * @retval 
     */
    R operator () (A... args) const {
        assert(_caller);
        return _caller(_object_ptr, std::forward<A>(args) ...);
    }

private:
    /** 
     * @brief type of the function caller pointer. 
     * @note   
     * @retval 
     */
    using Caller = R (*)(void*, A&& ...);

    using Deleter = void (*)(void*);

    /** 
     * @brief pointer to function caller which depends on the type in _object_ptr. 
     * @note   The
     * _caller contains a plain pointer to either function_caller, a
     * function_ptr_caller, a method_caller, a const_method_caller, or a
     * functor_caller. 
     * @retval None
     */
    Caller _caller = nullptr;

    /** 
     * @brief pointer to object held by the delegate: for plain function pointers it
     * is the function pointer, for class::methods it is a pointer to the class
     * instance, for functors it is a pointer to the shared_ptr _store
     * contents.  
     * @bnote
     * @retval None
     */
    void* _object_ptr = nullptr;

    /** 
     * @brief shared_ptr used to contain a memory object containing the callable, like
     * lambdas with closures, or our own wrappers.
     * @note   
     * @retval None
     */
    std::shared_ptr<void> _store;

    /** 
     * @brief private constructor for plain
     * @note   
     * @param  m: 
     * @param  obj: 
     * @retval 
     */
    basic_delegate(const Caller& m, void* const obj) WALLE_NOEXCEPT
        : _caller(m), _object_ptr(obj) 
    { 

    }

    /** 
     * @brief deleter for stored functor closures 
     * @note   
     * @param  ptr: 
     * @retval None
     */
    template <typename T>
    static void store_deleter(void* const ptr) 
    {
        using Rebind = typename Allocator::template rebind<T>::other;

        Rebind().destroy(static_cast<T*>(ptr));
        Rebind().deallocate(static_cast<T*>(ptr), 1);
    }

    /** 
     * @brief caller for an immediate function with no object or pointer.
     * @note   
     * @param  Function: 
     * @retval 
     */
    template <R(* Function)(A...)>
    static R function_caller(void* const, A&& ... args) 
    {
        return Function(std::forward<A>(args) ...);
    }

    /** 
     * @brief  caller for a plain function pointer. 
     * @note   
     * @param  object_ptr: 
     * @param  args: 
     * @retval 
     */
    static R function_ptr_caller(void* const object_ptr, A&& ... args) 
    {
        return (*reinterpret_cast<R(* const*)(A...)>(&object_ptr))(args...);
    }

    /** 
     * @brief function caller for immediate class::method function calls.
     * @note   
     * @param  method_ptr: 
     * @retval 
     */
    template <class C, R(C::* method_ptr)(A...)>
    static R method_caller(void* const object_ptr, A&& ... args) 
    {
        return (static_cast<C*>(object_ptr)->*method_ptr)(
            std::forward<A>(args) ...);
    }

    /** 
     * @brief function caller for immediate const class::method functions calls. 
     * @note   
     * @param  method_ptr: 
     * @retval 
     */
    template <class C, R(C::* method_ptr)(A...) const>
    static R const_method_caller(void* const object_ptr, A&& ... args) 
    {
        return (static_cast<C const*>(object_ptr)->*method_ptr)(
            std::forward<A>(args) ...);
    }

    /** 
     * @brief wrappers for indirect class::method calls containing (object,
     * method_ptr) 
     * @note   
     * @param  const: 
     * @retval None
     */
    template <class C>
    using MemberPair =
              std::pair<C* const, R(C::* const)(A...)>;

    /** 
     * @brief wrappers for indirect const class::method calls containing (object,
     * const method_ptr) 
     * @note   
     * @param  const: 
     * @retval None
     */
    template <class C>
    using ConstMemberPair =
              std::pair<C const* const, R(C::* const)(A...) const>;

    /** 
     * @brief template for class::function selector
     * @note   
     * @retval None
     */
    template <typename>
    struct IsMemberPair : std::false_type { 

    };

    /** 
     * @brief specialization for class::function selector 
     * @note   
     * @retval None
     */
    template <class C>
    struct IsMemberPair<MemberPair<C> >: std::true_type { 

    };

    /** 
     * @brief template for const class::function selector
     * @note   
     * @retval None
     */
    template <typename>
    struct IsConstMemberPair : std::false_type { 

    };

    /** 
     * @brief specialization for const class::function selector 
     * @note   
     * @retval None
     */
    template <class C>
    struct IsConstMemberPair<ConstMemberPair<C> >: std::true_type { 

    };

    /** 
     * @brief function caller for functor class. 
     * @note   
     * @param  IsConstMemberPair<T>::value: 
     * @retval 
     */
    template <typename T>
    static typename std::enable_if<
        !(IsMemberPair<T>::value || IsConstMemberPair<T>::value), R
        >::type
    functor_caller(void* const object_ptr, A&& ... args) 
    {
        return (*static_cast<T*>(object_ptr))(std::forward<A>(args) ...);
    }

    /** 
     * @brief  function caller for const functor class. 
     * @note   
     * @param  IsConstMemberPair<T>::value: 
     * @retval 
     */
    template <typename T>
    static typename std::enable_if<
        (IsMemberPair<T>::value || IsConstMemberPair<T>::value), R
        >::type
    functor_caller(void* const object_ptr, A&& ... args) 
    {
        return (static_cast<T*>(object_ptr)->first->*
                static_cast<T*>(object_ptr)->second)(std::forward<A>(args) ...);
    }

};

/** 
 * @brief make template alias due to similarity with std::function 
 * @note   
 * @retval None
 */
template <typename T, typename Allocator = std::allocator<void> >
using delegate = basic_delegate<T, Allocator>;

/** 
 * @brief constructor for wrapping a class::method with object pointer. 
 * @note   
 * @param  object_ptr: 
 * @param  method_ptr: 
 * @retval 
 */
template <class C, typename R, typename... A>
inline basic_delegate<R(A...)>
make_delegate(
    C* const object_ptr, R(C::* const method_ptr)(A...)) WALLE_NOEXCEPT 
{
    return basic_delegate<R(A...)>::template make<C>(object_ptr, method_ptr);
}

/** 
 * @brief constructor for wrapping a const class::method with object pointer. 
 * @note   
 * @param  object_ptr: 
 * @param  method_ptr: 
 * @retval 
 */
template <class C, typename R, typename... A>
inline basic_delegate<R(A...)>
make_delegate(
    C* const object_ptr, R(C::* const method_ptr)(A...) const) WALLE_NOEXCEPT 
{
    return basic_delegate<R(A...)>::template make<C>(object_ptr, method_ptr);
}

/** 
 * @brief constructor for wrapping a class::method with object reference. 
 * @note   
 * @param  object_ptr: 
 * @param  method_ptr: 
 * @retval 
 */
template <class C, typename R, typename... A>
inline basic_delegate<R(A...)>
make_delegate(
    C& object_ptr, R(C::* const method_ptr)(A...)) WALLE_NOEXCEPT {   // NOLINT
    return basic_delegate<R(A...)>::template make<C>(object_ptr, method_ptr);
}

/** 
 * @brief constructor for wrapping a const class::method with object reference. 
 * @note   
 * @param  object_ptr: 
 * @param  method_ptr: 
 * @retval 
 */
template <class C, typename R, typename... A>
inline basic_delegate<R(A...)>
make_delegate(
    C const& object_ptr, R(C::* const method_ptr)(A...) const) WALLE_NOEXCEPT 
{
    return basic_delegate<R(A...)>::template make<C>(object_ptr, method_ptr);
}
} //namespace walle
#endif //WALLE_COMMON_DELEGATE_H_