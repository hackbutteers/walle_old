#ifndef WALLE_CONFIG_BITS_H_
#define WALLE_CONFIG_BITS_H_
#include <walle/config/base.h>
#include <string.h>
// The type-based aliasing rule allows the compiler to assume that pointers of
// different types (for some definition of different) never alias each other.
// Thus the following code does not work:
//
// float f = foo();
// int fbits = *(int*)(&f);
//
// The compiler 'knows' that the int pointer can't refer to f since the types
// don't match, so the compiler may cache f in a register, leaving random data
// in fbits.  Using C++ style casts makes no difference, however a pointer to
// char data is assumed to alias any other pointer.  This is the 'memcpy
// exception'.
//
// Bit_cast uses the memcpy exception to move the bits from a variable of one
// type of a variable of another type.  Of course the end result is likely to
// be implementation dependent.  Most compilers (gcc-4.2 and MSVC 2005)
// will completely optimize BitCast away.
//
// There is an additional use for BitCast.
// Recent gccs will warn when they see casts that may result in breakage due to
// the type-based aliasing rule.  If you have checked that there is no breakage
// you can use BitCast to cast one pointer type to another.  This confuses gcc
// enough that it can no longer see that you have cast one pointer type to
// another thus avoiding the warning.
template <class Dest, class Source>
inline Dest bit_cast(const Source& source) 
{
  // Compile time assertion: sizeof(Dest) == sizeof(Source)
  // A compile error here means your Dest and Source have different sizes.
    static_assert(sizeof(Dest) == sizeof(Source), "");
    Dest dest;
    memmove(&dest, &source, sizeof(dest));
    return dest;
}

template <class Dest, class Source>
inline Dest bit_cast(Source* source) {
  return bit_cast<Dest>(reinterpret_cast<uintptr_t>(source));
}
#endif //WALLE_CONFIG_BITS_H_
