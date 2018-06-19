#ifndef WALLE_WSL_INTERNAL_SKIP_LIST_H_
#define WALLE_WSL_INTERNAL_SKIP_LIST_H_
#include <walle/config/base.h>
#include <cmath>
#include <cstdlib>

namespace wsl {
namespace sk_detail {
template <unsigned NumLevels>   class bit_based_skip_list_level_generator;
template <unsigned NumLevels>   class skip_list_level_generator;

template<bool> 
struct static_WALLE_ASSERT_impl;
template<> 
struct static_WALLE_ASSERT_impl<true> {};


template <unsigned NumLevels>
class skip_list_level_generator {
public:
    static const unsigned num_levels = NumLevels;
    unsigned new_level();
};

template <unsigned NumLevels>
class bit_based_skip_list_level_generator {
public:
    static const unsigned num_levels = NumLevels;
    unsigned new_level();
};

template <typename Compare, typename T>
inline
bool equivalent(const T &lhs, const T &rhs, const Compare &less)
{ 
    return !less(lhs, rhs) && !less(rhs, lhs); 
}

template <typename Compare, typename T>
inline
bool less_or_equal(const T &lhs, const T &rhs, const Compare &less)
{ 
    return !less(rhs, lhs);
}


template <unsigned ML>
inline
unsigned bit_based_skip_list_level_generator<ML>::new_level()
{
    WALLE_ASSERT_MSG(num_levels < 33, "num_levels must <= 32");

    unsigned level = 0;
    for (unsigned number = unsigned(rand()); (number & 1) == 1; number >>= 1) {
        level++;
    }
    return level;
}

template <unsigned ML>
inline
unsigned skip_list_level_generator<ML>::new_level()
{
    float f = float(std::rand())/float(RAND_MAX);
    unsigned level = unsigned(std::log(f)/std::log(0.5));
    return level < num_levels ? level : num_levels;
}

//sl_impl

template <typename T>
struct sl_node
{
    typedef sl_node<T> self_type;
    T           value;
    unsigned    level;
    self_type  *prev;
    self_type **next; 
};

template <typename T, typename Compare, typename Allocator,
          typename LevelGenerator, bool AllowDuplicates>
class sl_impl {
public:
    typedef T                                   value_type;
    typedef typename Allocator::size_type       size_type;
    typedef typename Allocator::difference_type difference_type;
    typedef typename Allocator::const_reference const_reference;
    typedef typename Allocator::const_pointer   const_pointer;
    typedef Allocator                           allocator_type;
    typedef Compare                             compare_type;
    typedef LevelGenerator                      generator_type;
    typedef sl_node<T>                          node_type;

    static const unsigned num_levels = LevelGenerator::num_levels;

    sl_impl(const Allocator &alloc = Allocator());
    ~sl_impl();

    Allocator        get_allocator() const                 { return alloc; }
    size_type        size() const                          { return item_count; }
    bool             is_valid(const node_type *node) const { return node && node != head && node != tail; }
    node_type       *front()                               { return head->next[0]; }
    const node_type *front() const                         { return head->next[0]; }
    node_type       *one_past_front()                      { return head; }
    const node_type *one_past_front() const                { return head; }
    node_type       *one_past_end()                        { return tail; }
    const node_type *one_past_end() const                  { return tail; }
    node_type       *find(const value_type &value) const;
    node_type       *find_first(const value_type &value) const;
    node_type       *insert(const value_type &value, node_type *hint = 0);
    void             remove(node_type *value);
    void             remove_all();
    void             remove_between(node_type *first, node_type *last);
    void             swap(sl_impl &other);
    size_type        count(const value_type &value) const;

    template <typename STREAM>
    void        dump(STREAM &stream) const;
    bool        check() const;
    unsigned    new_level();

    compare_type less;

private:
    typedef typename Allocator::template rebind<node_type>::other    node_allocator;
    typedef typename Allocator::template rebind<node_type*>::other   list_allocator;

    sl_impl(const sl_impl &other);
    sl_impl &operator=(const sl_impl &other);
    
    allocator_type  alloc;
    generator_type  generator;
    unsigned        levels;
    node_type      *head;
    node_type      *tail;
    size_type       item_count;
    
    node_type *allocate(unsigned level)
    {
        node_type *node = node_allocator(alloc).allocate(1, (void*)0);
        node->next  = list_allocator(alloc).allocate(level+1, (void*)0);
        node->level = level;
        return node;
    }

    void deallocate(node_type *node)
    {
        list_allocator(alloc).deallocate(node->next, node->level+1);
        node_allocator(alloc).deallocate(node, 1);
    }
};


template <class T, class C, class A, class LG, bool D>
inline
sl_impl<T,C,A,LG,D>::sl_impl(const allocator_type &alloc_)
:   alloc(alloc_),
    levels(0),
    head(allocate(num_levels)),
    tail(allocate(num_levels)),
    item_count(0)
{
    for (unsigned n = 0; n < num_levels; n++) {
        head->next[n] = tail;
        tail->next[n] = 0;
    }
    head->prev = 0;
    tail->prev = head;
}

template <class T, class C, class A, class LG, bool D>
inline
sl_impl<T,C,A,LG,D>::~sl_impl()
{
    remove_all();
    deallocate(head);
    deallocate(tail);
}

template <class T, class C, class A, class LG, bool D>
inline
typename sl_impl<T,C,A,LG,D>::size_type
sl_impl<T,C,A,LG,D>::count(const value_type &value) const
{
    // only used in multi_skip_lists
    WALLE_ASSERT(D);

    const node_type *node = find(value);
    size_type count = 0;

    // backwards (find doesn't necessarily land on the first)
    const node_type *back = node;
    if (back != head) {
        back = back->prev;
        while (back != head && sk_detail::equivalent(back->value, value, less)) {
            ++count;
            back = back->prev;
        }
    }

    // forwards
    while (is_valid(node) && sk_detail::equivalent(node->value, value, less)) {
        ++count;
        node = node->next[0];
    }
    return count;
}

template <class T, class C, class A, class LG, bool D>
inline
typename sl_impl<T,C,A,LG,D>::node_type *
sl_impl<T,C,A,LG,D>::find(const value_type &value) const
{
    // I could have an identical const and non-const overload,
    // but this cast is simpler (and safe)
    node_type *search = const_cast<node_type*>(head);

    for (unsigned l = levels; l; ) {
        --l;
        while (search->next[l] != tail && sk_detail::less_or_equal(search->next[l]->value, value, less)) {
            search = search->next[l];
        }
    }
    return search;
}
    
template <class T, class C, class A, class LG, bool D>
inline
typename sl_impl<T,C,A,LG,D>::node_type *
sl_impl<T,C,A,LG,D>::find_first(const value_type &value) const
{
    // only used in multi_skip_lists
    WALLE_ASSERT(D);

    node_type *node = find(value);
    
    while (node != head && sk_detail::equivalent(node->prev->value, value, less)) {
        node = node->prev;
    }
    if (node != tail && less(node->value, value)) 
        node = node->next[0];

    return node;
}

template <class T, class C, class A, class LG, bool AllowDuplicates>
inline
typename sl_impl<T,C,A,LG,AllowDuplicates>::node_type*
sl_impl<T,C,A,LG,AllowDuplicates>::insert(const value_type &value, node_type *hint)
{
    const unsigned level = new_level();

    node_type *new_node = allocate(level);
    WALLE_ASSERT(new_node);
    WALLE_ASSERT(new_node->level == level);
    alloc.construct(&new_node->value, value);

    const bool good_hint    = is_valid(hint) && hint->level == levels-1;
    node_type *insert_point = good_hint ? hint : head;
    unsigned   l            = levels;

    while (l) {
        --l;
        WALLE_ASSERT(l <= insert_point->level);
        while (insert_point->next[l] != tail && less(insert_point->next[l]->value, value)) {
            insert_point = insert_point->next[l];
            WALLE_ASSERT(l <= insert_point->level);
        }
        
        if (l <= level) {
            node_type *next = insert_point->next[l];
            WALLE_ASSERT(next);
        
            new_node->next[l]     = next;
            insert_point->next[l] = new_node;
        }
    }
    
    WALLE_ASSERT(insert_point->next[0] == new_node);
    node_type *next = new_node->next[0];
    WALLE_ASSERT(next);
    new_node->prev = insert_point;
    next->prev = new_node;

    ++item_count;
          


    // Do not allow repeated values in the list
    if (!AllowDuplicates && next != tail && sk_detail::equivalent(next->value, value, less)) {
        remove(new_node);
        new_node = tail;
    }

    return new_node;
}

template <class T, class C, class A, class LG, bool AllowDuplicates>
inline
void
sl_impl<T,C,A,LG,AllowDuplicates>::remove(node_type *node)
{
    WALLE_ASSERT(is_valid(node));
    WALLE_ASSERT(node->next[0]);

    node->next[0]->prev = node->prev;

    // patch up all next pointers
    node_type *cur = head;
    for (unsigned l = levels; l; ) {
        --l;
        WALLE_ASSERT(l <= cur->level);
        while (cur->next[l] != tail && less(cur->next[l]->value, node->value)) {
            cur = cur->next[l];
        }
        if (AllowDuplicates) {
            node_type *cur2 = cur;

            while (cur2 != tail) {
                node_type *next = cur2->next[l];
                if (next == tail) break;
                if (next == node) {
                    cur = cur2;
                    break;
                }
                if (sk_detail::equivalent(cur2->next[l]->value, node->value, less))
                    cur2 = next;
                else
                    break;
            }
        }
        if (cur->next[l] == node) {
            cur->next[l] = node->next[l];
        }
    }

    alloc.destroy(&node->value);
    deallocate(node);

    item_count--;
    
}

template <class T, class C, class A, class LG, bool D>
inline
void
sl_impl<T,C,A,LG,D>::remove_all()
{
    node_type *node = head->next[0];
    while (node != tail) {
        node_type *next = node->next[0];
        alloc.destroy(&node->value);
        deallocate(node);
        node = next;
    }

    for (unsigned l = 0; l < num_levels; ++l)
        head->next[l] = tail;
    tail->prev = head;
    item_count = 0;
        
}

template <class T, class C, class A, class LG, bool D>
inline
void 
sl_impl<T,C,A,LG,D>::remove_between(node_type *first, node_type *last)
{
    WALLE_ASSERT(is_valid(first));
    WALLE_ASSERT(is_valid(last));
    WALLE_ASSERT(!D);

    node_type       * const prev         = first->prev;
    node_type       * const one_past_end = last->next[0];
    const value_type       &first_value  = first->value;
    const value_type       &last_value   = last->value;

    // backwards pointer
    one_past_end->prev = prev;

    // forwards pointers
    node_type *cur = head;
    for (unsigned l = levels; l; ) {
        --l;
        WALLE_ASSERT(l < cur->level);
        while (cur->next[l] != tail && less(cur->next[l]->value, first_value)) {
            cur = cur->next[l];
        }
        if (cur->next[l] != tail
            && sk_detail::less_or_equal(cur->next[l]->value, last_value, less)) {
            // patch up next[l] pointer
            node_type *end = cur->next[l];
            while (end != tail && sk_detail::less_or_equal(end->value, last_value, less))
                end = end->next[l];
            cur->next[l] = end;
        }
    }

    // now delete all the nodes between [first,last]
    while (first != one_past_end) {
        node_type *next = first->next[0];
        alloc.destroy(&first->value);
        deallocate(first);
        item_count--;
        first = next;
    }
    
}

template <class T, class C, class A, class LG, bool D>
inline
unsigned sl_impl<T,C,A,LG,D>::new_level()
{    
    unsigned level = generator.new_level();
    if (level >= levels) {
        level = levels;
        ++levels;
    }
    return level;
}

template <class T, class C, class A, class LG, bool D>
inline
void sl_impl<T,C,A,LG,D>::swap(sl_impl &other)
{
    using std::swap;

    swap(alloc,      other.alloc);
    swap(less,       other.less);
    swap(generator,  other.generator);
    swap(levels,     other.levels);
    swap(head,       other.head);
    swap(tail,       other.tail);
    swap(item_count, other.item_count);

}

// for diagnostics only
template <class T, class C, class A, class LG, bool AllowDuplicates>
template <class STREAM>
inline
void sl_impl<T,C,A,LG,AllowDuplicates>::dump(STREAM &s) const
{
    s << "skip_list(size="<<item_count<<",levels=" << levels << ")\n";
    for (unsigned l = 0; l < levels+1; ++l) {
        s << "  [" << l << "]" ;
        const node_type *n = head;
        while (n) {
            WALLE_ASSERT(l <= n->level);
            const node_type *next = n->next[l];
            bool prev_ok = false;
            if (next) {
                if (next->prev == n) prev_ok = true;
            }
            if (is_valid(n))
                s << n->value;
            else
                s << "*";
            
            if (is_valid(n)) {
                if (AllowDuplicates) {
                    if (next != tail && !sk_detail::less_or_equal(n->value, next->value, less))
                        s << "*XXXXXXXXX*";
                } else {
                    if (next != tail && !less(n->value, next->value))
                        s << "*XXXXXXXXX*";
                }
            }
            s << "> ";
            if (is_valid(n))
            {
              s << (prev_ok?"<":"-");
            }
            n = next;
        }
        s << "\n";
    }
}


template <typename LIST> class sl_iterator;
template <typename LIST> class sl_const_iterator;

template <typename SL_IMPL>
class sl_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SL_IMPL::value_type,
                           typename SL_IMPL::difference_type,
                           typename SL_IMPL::const_pointer,
                           typename SL_IMPL::const_reference> {
public:
    typedef SL_IMPL                         impl_type;
    typedef sl_const_iterator<impl_type>    const_iterator;
    typedef typename impl_type::node_type   node_type;
    typedef sl_iterator<impl_type>          self_type;

    typedef typename impl_type::const_reference const_reference;
    typedef typename impl_type::const_pointer   const_pointer;

    sl_iterator() :
        _node(0) {}

    sl_iterator(node_type *node) :
        _node(node) {}

    self_type &operator++()
    { 
        _node = _node->next[0]; 
        return *this; 
    }
    self_type operator++(int) // postincrement
    { 
        self_type old(*this); 
        _node = _node->next[0]; 
        return old; 
    }

    self_type &operator--()
    { 
        _node = _node->prev; 
        return *this; 
    }
    self_type operator--(int) // postdecrement
    { 
        self_type old(*this); 
        _node = _node->prev; 
        return old; 
    }

    const_reference operator*()  
    { 
        return _node->value; 
    }
    const_pointer   operator->() 
    { 
        return _node->value; 
    }
    
    bool operator==(const self_type &other) const
    { 
        return _node == other._node; 
    }
    bool operator!=(const self_type &other) const
    { 
        return !operator==(other);
    }
    
    bool operator==(const const_iterator &other) const
    { 
        return _node == other.get_node(); 
    }
    bool operator!=(const const_iterator &other) const
    { 
        return !operator==(other); 
    }

    const node_type *get_node() const 
    { 
        return _node; 
    }

private:
    node_type   *_node;
};

template <class SL_IMPL>
class sl_const_iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           typename SL_IMPL::value_type,
                           typename SL_IMPL::difference_type,
                           typename SL_IMPL::const_pointer,
                           typename SL_IMPL::const_reference> {
public:
    typedef const SL_IMPL                       impl_type;
    typedef sl_iterator<SL_IMPL>                iterator;
    typedef const typename impl_type::node_type node_type;
    typedef sl_const_iterator<SL_IMPL>          self_type;

    typedef typename impl_type::const_reference const_reference;
    typedef typename impl_type::const_pointer   const_pointer;

    sl_const_iterator() : _node(0) {}
    sl_const_iterator(const iterator &i) :_node(i.get_node()) {}

    self_type &operator++()
    { 
        _node = _node->next[0]; 
        return *this; 
    }
    self_type operator++(int) // postincrement
    { 
        self_type old(*this); 
        _node = _node->next[0]; 
        return old; 
    }

    self_type &operator--()
    { 
        _node = _node->prev; 
        return *this; 
    }

    self_type operator--(int) // postdecrement
    { 
        self_type old(*this); 
        _node = _node->prev; 
        return old; 
    }

    const_reference operator*()  
    { 
        return _node->value; 
    }
    const_pointer   operator->() 
    { 
        return _node->value; 
    }

    bool operator==(const self_type &other) const
    { 
        return _node == other._node; 
    }
    bool operator!=(const self_type &other) const
    { 
        return !operator==(other); 
    }

    bool operator==(const iterator &other) const
    { 
        return _node == other.get_node(); 
    }
    bool operator!=(const iterator &other) const
    { 
        return !operator==(other);
    }

    const node_type *get_node() const 
    { 
        return _node; 
    } 

private:
    node_type *_node;
};


} //namespace sk_detail 
} //namespace wsl

#endif