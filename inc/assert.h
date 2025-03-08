#ifndef ASSERT_H
#define ASSERT_H

#include <stdio.h>
#include <stdlib.h>

// Input is __FILE__ and __LINE__ from the assert macro
void assert(const char *file, int line) __attribute__((optimize("-O3")));

#endif // ASSERT_H