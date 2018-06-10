#include <walle/memory/pool.h>
#include <walle/memory/malloc_tracker.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <deque>
#include <functional>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cstdio>

using namespace walle::mem;


TEST(MemPool, abc) 
{
    Pool pool;
    using IntAlloc = PoolAllocator<int>;
    IntAlloc al(pool);
    (void)al;
    
    int * p2 = al.allocate(1);
    al.deallocate(p2, 1);
        
    int * p3 = al.allocate(128);
    al.deallocate(p3,128);
    
    
    int * p4 = al.allocate(128);
    al.deallocate(p4, 128);
    
    int * p5 = al.allocate(128);
    
    int * p6 = al.allocate(21);
    int * p7 = al.allocate(43);
    int * p8 = al.allocate(143);
   
    
    
    al.deallocate(p5, 128);
    
    al.deallocate(p6, 21);
    al.deallocate(p7, 43);
    al.deallocate(p8, 143);
    
     
}

TEST(MemPool, RandomAllocDealloc) {
    Pool pool;
    std::default_random_engine rng(std::random_device { } ());

    size_t max_size = 256;
    size_t iterations = 10000;

    std::deque<std::pair<void*, size_t> > list;

    while (iterations != 0) {
        size_t op = rng() % 2;

        if (op == 0) {
            // allocate a memory piece
            --iterations;
            size_t size = (rng() % max_size) + 1;
            list.emplace_back(pool.allocate(size), size);
        } else if (op == 1 && !list.empty()) {
            // deallocate a memory piece
            pool.deallocate(list.front().first, list.front().second);
            list.pop_front();
        }

        pool.self_verify();
    }

    while (!list.empty()) {
        pool.deallocate(list.front().first, list.front().second);
        list.pop_front();

        pool.self_verify();
    }

    pool.self_verify();
    
}

TEST(MemPool, Vector) {
    Pool pool(1024);
    using IntAlloc = PoolAllocator<int>;

    std::vector<int, IntAlloc> my_vector {
        IntAlloc(pool)
    };
    
    for (int i = 0; i < 100; ++i) {
        my_vector.push_back(i);
    }
    
    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(i, my_vector[i]);
    }
    
}

TEST(MemPool, Deque) {
    Pool pool(16384);
    using IntAlloc = PoolAllocator<int>;

    std::deque<int, IntAlloc> my_deque {
        IntAlloc(pool)
    };

    for (int i = 0; i < 100; ++i) {
        my_deque.push_back(i);
    }

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(i, my_deque[i]);
    }
    
}

TEST(MemPool, Set) {
    Pool pool(256);
    using IntAlloc = PoolAllocator<int>;

    std::set<int, std::less<int>, IntAlloc> my_set {
        IntAlloc(pool)
    };

    for (int i = 0; i < 100; ++i) {
        my_set.insert(i);
    }

    int i = 0;
    for (const int& it : my_set) {
        ASSERT_EQ(i, it);
        ++i;
    }
    
}

TEST(MemPool, SetRandom) {
    Pool pool(256);
    using IntAlloc = PoolAllocator<int>;

    std::set<int, std::less<int>, IntAlloc> my_set {
        IntAlloc(pool)
    };
    std::deque<int> check_set;

    // std::default_random_engine rng(std::random_device { } ());
    std::default_random_engine rng(1234);

    size_t limit = 1000;
    for (size_t i = 0; i < limit; ++i) {
        size_t op = rng() % 2;
        if (op == 0 && limit > i + check_set.size())
        {
            int v = static_cast<int>(rng());
            my_set.insert(v);
            check_set.insert(
                std::lower_bound(check_set.begin(), check_set.end(), v), v);
        }
        else if (check_set.size())
        {
            size_t idx = rng() % check_set.size();
            auto it = check_set.begin() + idx;

            ASSERT_TRUE(my_set.count(*it));
            my_set.erase(*it);
            check_set.erase(it);
        }

        ASSERT_EQ(check_set.size(), my_set.size());
        auto it1 = my_set.begin();
        auto it2 = check_set.begin();
        while (it2 != check_set.end()) {
            ASSERT_TRUE(it1 != my_set.end());
            ASSERT_EQ(*it2, *it1);
            ++it1, ++it2;
        }
    }
    
}

TEST(MemPool, UnorderedMap) {
    Pool pool(4096);
    using IntAlloc =
              PoolAllocator<std::pair<const int, std::string> >;

    std::unordered_map<int, std::string,
                       std::hash<int>, std::equal_to<int>,
                       IntAlloc> my_map {
        IntAlloc(pool)
    };

    for (int i = 0; i < 100; ++i) {
        my_map[i * i] = std::to_string(i);
    }

    std::vector<std::pair<int, std::string> > my_vec(
        my_map.begin(), my_map.end());

    std::sort(my_vec.begin(), my_vec.end());

    for (size_t i = 0; i != my_vec.size(); ++i) {
        ASSERT_EQ(i * i, my_vec[i].first);
        ASSERT_EQ(std::to_string(i), my_vec[i].second);
    }
    
}

namespace walle {
namespace mem {

template class PoolAllocator<int>;

} // namespace mem
} // namespace walle

