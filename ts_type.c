#include "ts_type.h"
#include "ts_misc.h"
#include <stdlib.h>
#include <time.h>

static time_t localtime_offset;

char *type_to_str(ts_type_t vtype)
{
  TS_TRY(vtype < TSTYPE_MAX);
  static char *str[] = {
    "",
    "any",
    "dot",
    "var",
    "varid",
    "svarid",
    "pvarid",
    "int",
    "float",
    "double",
    "str",
    "sym",
    "time",
  };

  return str[vtype];
 fail:
  return 0;
}

char *ts_time_to_str(ts_time_t t)
{
  static char buf[100];
  struct tm tm;
  time_t tt = 86400*t;
  gmtime_r((time_t*)&tt, &tm);
  sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  return buf;
}

int ts_time_get_month(ts_time_t t)
{
  TS_TRY(t);
  struct tm tm;
  time_t tt = 86400*t;
  gmtime_r((time_t*)&tt, &tm);
  return (tm.tm_year+1900)*100 + (tm.tm_mon+1);
 fail:
  return 0;
}

int ts_time_get_day(ts_time_t t)
{
  TS_TRY(t);
  struct tm tm;
  time_t tt = 86400*t;
  gmtime_r((time_t*)&tt, &tm);
  return (tm.tm_year+1900)*10000 + (tm.tm_mon+1)*100 + tm.tm_mday;
 fail:
  return 0;
}

ts_time_t my_strptime(char *s, const char *fmt)
{
  TS_TRY(s && fmt);
  struct tm tm;
  int Y=0,y=0,M=0,d=0,h=0,m=0;
  double ss=0;
  const char *p = fmt;
  char a;
  memset(&tm, 0, sizeof(struct tm));
  
  while(*p) {
    switch(*p){
    case '%':
      a = *++p;
      p++;
      switch(a) {
      case 'Y':
	Y = strntoi(s, &s, 4);
	break;
      case 'y':
	Y = strntoi(s, &s, 2);
	break;
      case 'M':
	M = strntoi(s, &s, 2);
	break;
      case 'd':
	d = strntoi(s, &s, 2);
	break;
      case 'h':
	h = strntoi(s, &s, 2);
	break;
      case 'm':
	m = strntoi(s, &s, 2);
	break;
      case 's':
	ss = strtod(s, &s);
	break;
      default:
	(void)ts_warn(stderr, "\n");
	goto fail;
	break;
      }
      break;
    default:
      p++;
      s++;
      break;
    }
  }
  if(Y != 0) {
    tm.tm_year = Y - 1900;
    tm.tm_mon = M-1;
    tm.tm_mday = d;
    return mktime_internal(&tm, gmtime_r, &localtime_offset)/86400.0 + h/24.0 + m/1440.0 + ss/86400.0;
  } else {
    tm.tm_year = y;
    tm.tm_mon = M-1;
    tm.tm_mday = d;
    return mktime_internal(&tm, gmtime_r, &localtime_offset)/86400.0 + h/24.0 + m/1440.0 + ss/86400.0;
  }

 fail:
  return 0.0;
}

double ts_time_const_to_double(char *t)
{
  double v,s=0.0;
  while(t && *t) {
    v = strtod(t, &t);
    TS_TRY(t && *t);
    switch(*t) {
    case 'd':
      s += v;
      break;
    case 'h':
      s += v/24.0;
      break;
    case 'm':
      s += v/1440.0;
      break;
    case 's':
      s += v/86400.0;
      break;
    default:
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
    t++;
  }
  return s;
 fail:
  return 0.0;
}

int ts_type_size(ts_type_t t)
{
  switch(t) {
  case TSTYPE_INT:
    return sizeof(int);
  case TSTYPE_FLOAT:
    return sizeof(float);
  case TSTYPE_DOUBLE:
    return sizeof(double);
  case TSTYPE_SYM:
    return sizeof(char*);
  case TSTYPE_TIME:
    return sizeof(ts_time_t);
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
 fail:
  return 0;
}

int ts_time_sub_to_subi(const char *sub, short *subi)
{
  TS_TRY(sub);
  if(0 == strcmp("date", sub)) {
    *subi = 1;
  } else if(0 == strcmp("hour", sub)) {
    *subi = 2;
  } else if(0 == strcmp("min", sub)) {
    *subi = 3;
  } else if(0 == strcmp("second", sub)) {
    *subi = 4;
  } else if(0 == strcmp("hms", sub)) {
    *subi = 5;
  }
  return 1;
 fail:
  return 0;
}

int ts_time_get_subi(ts_time_t t, int sub)
{
  TS_TRY(sub > 0);
  struct tm tm;
  time_t tt = t*86400;
  gmtime_r(&tt, &tm);
  switch(sub) {
  case 1:
    return (tm.tm_year + 1900)*10000 + (tm.tm_mon+1)*100 + tm.tm_mday;
    break;
  case 2:
    return tm.tm_hour;
    break;
  case 3:
    return tm.tm_min;
    break;
  case 4:
    return tm.tm_sec;
    break;
  case 5:
    return tm.tm_hour*10000 + tm.tm_min*100 + tm.tm_sec;
    break;
  default:
    (void)ts_warn(stderr, "get time sub error.\n");
    goto fail;
  }
  return 1;
 fail:
  return 0;
}

