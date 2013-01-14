#include <assert.h>

/* Useful assertions */

#define ASSERT(condition, message) do { \
unsigned int result;\
if (!(result = condition)) { fprintf(stderr,(message"\n")); } \
assert ((result)); } while(0)


