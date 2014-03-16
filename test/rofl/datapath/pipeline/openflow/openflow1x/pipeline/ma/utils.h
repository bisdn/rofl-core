#ifndef PROFILING_UTILS
#define PROFILING_UTILS

#include <rofl.h>

//Extern C
ROFL_BEGIN_DECLS


//From http://stackoverflow.com/questions/6814792/why-is-clock-gettime-so-erratic
static inline uint64_t rdtsc() {
	#if defined(__GNUC__)
	#   if defined(__i386__)
	    uint64_t x;
	    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	    return x;
	#   elif defined(__x86_64__)
	    uint32_t hi, lo;
	    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	    return ((uint64_t)lo) | ((uint64_t)hi << 32);
	#   else
	#       error Unsupported architecture.
	#   endif
	#elif defined(_MSC_VER)
	    return __rdtsc();
	#else
	#   error Other compilers not supported...
	#endif
}

//Extern C
ROFL_END_DECLS


#endif
