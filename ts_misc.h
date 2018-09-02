#ifndef TS_MISC_H
#define TS_MISC_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max3
#define max3(a, b, c) (((a) > (b)) ? (((a) > (c)) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))
#endif

#ifndef ts_warn
#define ts_warn(stream, ...) (fprintf(stream, "%s:%s:%d\t", __FILE__, __FUNCTION__, __LINE__) + fprintf(stream, __VA_ARGS__) + ts_warn_break())
#define ts_warn_sys_lib() ts_warn(stderr, "%s\n", strerror(errno))
#define TS_TRY(eval) do{if(!(eval)){ (void)ts_warn(stderr, "%s\n", #eval ); goto fail; }}while(0)
#endif

#ifndef ts_free
#define ts_free(val) if((val)) free((val)); (val) = 0;
#endif

#ifndef ts_realloc
#define ts_realloc(ptr, m, n, TYPE)  {					\
    if(!((ptr) = realloc((ptr), ((m)+(n))*sizeof(TYPE)))) {		\
      (void)ts_warn(stderr, "realloc failed.\n");			\
      goto fail;							\
    }									\
    memset((((char*)(ptr))+(m)*sizeof(TYPE)), 0, ((n)*sizeof(TYPE)));	\
  }									\

#endif

inline int intcmp(const void*v1, const void*v2);
int ts_warn_break(void);
int ts_string_trim(char* str);
void *memdup(const void *src, size_t len);
time_t
mktime_internal (struct tm *tp,
		   struct tm *(*convert) (const time_t *, struct tm *),
		   time_t *offset);

inline int strntoi(char *buf, char **p, int n);

#endif
