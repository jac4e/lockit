#include "assert.h"
#include "log.h"

void __attribute__((optimize("-O3"))) assert(const char *file, int line)
{
    ERROR("Assertion failed at %s:%d", file, line);
    for(;;);
}