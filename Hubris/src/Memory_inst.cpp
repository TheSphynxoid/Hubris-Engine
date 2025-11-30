#include "Memory.h"

/**
 * This file is here to pre-instantiate all varient of Shared, Handle and Weak to avoid it being instatiated multiple times.
 */

using namespace Hubris;

/**
 * @brief To avoid bloat, since Weak<T> itself will cause Shared<T> I pre-instantiate only one of them.
 * While it is possible that all of these pre-instantiated types are useless, it's not a loss to have them here.
 * 
 */
#define INSTANTIATE(type) \
template struct Weak<type>;\
template struct Weak<const type>;\
template struct Handle<type>;\
template struct Handle<const type>;

INSTANTIATE(char);
INSTANTIATE(signed char);
INSTANTIATE(unsigned char);
INSTANTIATE(short);
INSTANTIATE(unsigned short);
INSTANTIATE(int);
INSTANTIATE(unsigned int);
INSTANTIATE(long);
INSTANTIATE(unsigned long);
INSTANTIATE(long long);
INSTANTIATE(unsigned long long);

INSTANTIATE(float);
INSTANTIATE(double);
INSTANTIATE(long double);

INSTANTIATE(bool);
INSTANTIATE(size_t);
INSTANTIATE(ptrdiff_t);
INSTANTIATE(wchar_t);
INSTANTIATE(char16_t);
INSTANTIATE(char32_t);

