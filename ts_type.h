#ifndef TS_TYPE_H
#define TS_TYPE_H

typedef enum ts_type {
  TSTYPE_NONE = 0,
  TSTYPE_ANY,
  TSTYPE_DOT, // tablename & varname
  TSTYPE_VARS, //any var string
  TSTYPE_VARI, //table var id
  TSTYPE_SVARI, //single var id
  TSTYPE_INT,
  TSTYPE_FLOAT,
  TSTYPE_DOUBLE,
  TSTYPE_STR,
  TSTYPE_SYM,
  TSTYPE_TIME,
  TSTYPE_MAX,
} ts_type_t;

typedef double ts_time_t;

ts_time_t my_strptime(char *s, const char *fmt);

int ts_time_get_month(ts_time_t t);
int ts_time_get_day(ts_time_t t);

char *type_to_str(ts_type_t vtype);

char *ts_time_to_str(ts_time_t t);
double ts_time_const_to_double(char *t);
int ts_time_sub_to_subi(const char *sub, short *subi);
int ts_time_get_subi(ts_time_t t, int subi);
int ts_type_size(ts_type_t t);
#endif
