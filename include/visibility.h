#if defined(__GNUC__) && (defined(__ELF__))
#  define VISIBILITY(x) __attribute__ ((visibility (x)))
#else
#  define VISIBILITY(x)
#endif
