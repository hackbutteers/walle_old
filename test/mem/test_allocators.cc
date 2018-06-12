#include <walle/memory/allocator.h>
#include <walle/memory/aligned_allocator.h>
#include <google/gtest/gtest.h>
#include <cstdlib>
#include <cstdio>

TEST(mem, allocator)
{
    printf("%lu\n", walle::mem::malloc_tracker_total_allocs());
    walle::mem::AlignedAllocator<char> al;
    char *ptr = al.allocate(1024);
    al.deallocate(ptr, 1024);
    walle::mem::ManagerPtr m(new walle::mem::Manager(walle::mem::ManagerPtr(), "test"));
    walle::mem::Allocator<char> alloctor1(m);
    printf("%lu\n", walle::mem::malloc_tracker_total_allocs());
    ptr = alloctor1.allocate(1024);
    alloctor1.deallocate(ptr, 1024);
    printf("%lu\n", walle::mem::malloc_tracker_total_allocs());
    {
        /*
        walle::mem::string a{walle::mem::Allocator<char>(m)};
        a.append(8092, 'c');
        (void)a;
        walle::mem::by_string b("24");
        b.append(8092, 'c');
        (void)b;
        */
    }
    printf("%lu\n", walle::mem::malloc_tracker_total_allocs());
}