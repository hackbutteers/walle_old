#include <walle/common/delegate.h>
#include <google/gtest/gtest.h>
#include <cstdlib>

using walle::Delegate;

using TestDelegate = Delegate<int(int)>;

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
        TestDelegate d = TestDelegate::make<func1>();
        EXPECT_EQ(42, d(37));
    }
    {
        // construction from a plain function pointer.
        TestDelegate d = TestDelegate(func1);
        EXPECT_EQ(42, d(37));
    }
    {
        // construction from a plain function pointer.
        TestDelegate d = TestDelegate::make(func1);
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
        TestDelegate d = TestDelegate::make<A, &A::func>(&a);
        EXPECT_EQ(42, d(40));
    }
    {
        // construction for an immediate class::method with class object
        TestDelegate d = TestDelegate::make<A, &A::const_func>(&a);
        EXPECT_EQ(42, d(40));
    }
    {
        // construction for an immediate class::method with class object by
        // reference
        TestDelegate d = TestDelegate::make<A, &A::func>(a);
        EXPECT_EQ(42, d(40));
    }
    {
        // construction for an immediate class::method with class object by
        // reference
        TestDelegate d = TestDelegate::make<A, &A::const_func>(a);
        EXPECT_EQ(42, d(40));
    }

    {
        // constructor from an indirect class::method with object pointer.
        TestDelegate d = TestDelegate(&a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object pointer.
        TestDelegate d = TestDelegate(&a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        TestDelegate d = TestDelegate(a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        TestDelegate d = TestDelegate(a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }

    {
        // constructor from an indirect class::method with object pointer.
        TestDelegate d = TestDelegate::make(&a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object pointer.
        TestDelegate d = TestDelegate::make(&a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        TestDelegate d = TestDelegate::make(a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        TestDelegate d = TestDelegate::make(a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }

    {
        // constructor from an indirect class::method with object pointer.
        TestDelegate d = walle::make_delegate(&a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object pointer.
        TestDelegate d = walle::make_delegate(&a, &A::const_func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        TestDelegate d = walle::make_delegate(a, &A::func);
        EXPECT_EQ(42, d(40));
    }
    {
        // constructor from an indirect class::method with object reference.
        TestDelegate d = walle::make_delegate(a, &A::const_func);
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
        TestDelegate d = TestDelegate(f);
        EXPECT_EQ(42, d(30));
    }
    {
        // calls general functor constructor
        TestDelegate d = TestDelegate::make(f);
        EXPECT_EQ(42, d(30));
    }
}

TEST(delgate,test_lambdas) 
{

    {
        TestDelegate d = TestDelegate([](int x) { return x + 1; });
        EXPECT_EQ(42, d(41));
    }
    {
        TestDelegate d = TestDelegate::make([](int x) { return x + 1; });
        EXPECT_EQ(42, d(41));
    }
    {
        // test a lambda with capture
        int val = 10;
        TestDelegate d = TestDelegate::make([&](int x) { return x + val; });
        EXPECT_EQ(42, d(32));
    }
    {
        TestDelegate d = [](int x) { return x + 1; };
        EXPECT_EQ(42, d(41));
    }
}
