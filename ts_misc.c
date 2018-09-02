#include "ts_misc.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <alloca.h>

int ts_warn_break(void)
{
  return 0;
}

int ts_string_trim(char *str)
{
  int len;
  char *buf=0;
  char *str_beg=0;
  char *str_end=0;

  if(0==str) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  
  if(0 == *str) {
    return(1);
  }
	
  len=1+strlen(str);
  buf=(char *)alloca(len * sizeof(char));
  if(0==buf) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  (void)memset(buf, 0, len * sizeof(char));

  str_end=str+strlen(str); 
  while(str_end>=str && (' ' == *str_end || '\t' == *str_end || '\n' == *str_end || '\0' == *str_end)) {
    *str_end='\0';
    str_end--;
  }
  
  if('\0' == *str) {
    return(1);
  }

  str_beg=str;
  str_end=str+strlen(str); 
  while(str_beg<=str_end && (' ' == *str_beg || '\t' == *str_beg || '\n' == *str_beg)) {
    str_beg++;
  }

  (void)strcpy(buf, str_beg);
  (void)memcpy(str, buf, len * sizeof(char));
	
  return(1);

 fail:
  return(0);
}

inline int intcmp(const void*v1, const void*v2)
{
  return *(int*)v1 - *(int*)v2;
}

void *memdup(const void *src, size_t len)
{
  void *dst = malloc(len);
  if(dst)
    memcpy(dst, src, len);
  return dst;
}

inline int strntoi(char *buf, char **p, int n)
{
  int result=0;
  while(n--) {
    result *=10;
    result += (*buf++) - '0';
  }
  *p = buf;
  return result;
}
