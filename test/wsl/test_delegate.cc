#include <walle/wsl/delegate.h>
#include <google/gtest/gtest.h>
#include <cstdlib>

using wsl::delegate;

using test_delegate = delegate<int(int)>;

int func1(int a) 
{
    return a + 5;
}

int func2(int a) 
{
    return a + 10;
}

TEST(delegate, test_plain_functions)
{
    {
        // construction from a immediate function with no object or pointer.
        test_delegate d = test_delegate::make<func1>();
        EXPECT_EQ(42, d(37));
    }
    {
        // construction from a plain function pointer.
        test_delegate d = test_delegate(func1);
        EXPECT_EQ(42, d(37));
    }
    {
        // construction from a plain function pointer.
        test_delegate d = test_delegate::make(func1);
        EXPECT_EQ(42, d(37));
    }
}

class A {
public:
    int x;

    int func(int a) 
    {
        return a + x;
    }

    int const_func(int a) const 
    {
        return a + x;
    }

    int func2(int a) 
    {
        return a + x + x;
    }
};

TEST(delegate, test_class_methods) 
{

    A a = { 2 };
    {
        // construction for an immediate class::method with class object
        test_delegate d = test_delegate::make<A, &A::func>(&a);
        EXPECT_EQ(42, d(40));
    }
    {
        // construction for an immediate class::method with class object
        test_delegate d = test_delegate::make<A, &A::const_func>(&a);
        EXPECT_EQ(42, d(40));
    }
    {
        // construction for an immediate class::method with class object by
        // reference
        test_delegate d = test_delegate::make<A, &A::func>(a);
        EXPECT_EQ(42, d(40));
    }
    {
        // construction for an immediate class::method with class object by
        // reference
        test_delegate d = test_delegate::make<A, &A::const_func>(a);
        EXPECT_EQ(42, d(40));
    }

    {
        // constructor from an indirect class::method with object pointer.
        test_delegate d = test_delegate(&a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object pointer.
        test_delegate d = test_delegate(&a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        test_delegate d = test_delegate(a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        test_delegate d = test_delegate(a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }

    {
        // constructor from an indirect class::method with object pointer.
        test_delegate d = test_delegate::make(&a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object pointer.
        test_delegate d = test_delegate::make(&a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        test_delegate d = test_delegate::make(a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        test_delegate d = test_delegate::make(a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }

    {
        // constructor from an indirect class::method with object pointer.
        test_delegate d = wsl::make_delegate(&a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object pointer.
        test_delegate d = wsl::make_delegate(&a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        test_delegate d = wsl::make_delegate(a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        test_delegate d = wsl::make_delegate(a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }
}

class AddFunctor {
public:
    int x;

    int operator () (int a) 
    {
        return a + x;
    }
};

TEST(delgate, test_functor_class)
{

    AddFunctor f = { 12 };

    {
        // calls general functor constructor
        test_delegate d = test_delegate(f);
        EXPECT_EQ(42, d(30));
    }
    {
        // calls general functor constructor
        test_delegate d = test_delegate::make(f);
        EXPECT_EQ(42, d(30));
    }
}

TEST(delgate,test_lambdas) 
{

    {
        test_delegate d = test_delegate([](int x) { return x + 1; });
        EXPECT_EQ(42, d(41));
    }
    {
        test_delegate d = test_delegate::make([](int x) { return x + 1; });
        EXPECT_EQ(42, d(41));
    }
    {
        // test a lambda with capture
        int val = 10;
        test_delegate d = test_delegate::make([&](int x) { return x + val; });
        EXPECT_EQ(42, d(32));
    }
    {
        test_delegate d = [](int x) { return x + 1; };
        EXPECT_EQ(42, d(41));
    }
}
