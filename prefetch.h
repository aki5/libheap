
#if defined PORT
#define prefetch(x)
#elif defined __arm__ || defined arm
#define prefetch(x)
#elif defined __amd64__ || defined __x86_64__ || defined x86_64 || defined __x86_64
#include <xmmintrin.h>
#define prefetch(x) _mm_prefetch(x, _MM_HINT_T0)
#else
#define prefetch(x)
#endif

