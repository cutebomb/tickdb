#ifndef _TS_CSV_READER
#define _TS_CSV_READER

#define ELEM_LEN 200

#include <stdlib.h>
#include <stdio.h>
#include "ts_misc.h"
#include "ts_file.h"

typedef struct ts_csv_reader {
  ts_file_t *file;
  int head_readed;
  int ncol;
  char **elem_ar;
  char **head_ar;
} ts_csv_reader_t;

ts_csv_reader_t *ts_csv_reader_new(const char *filename);
int ts_csv_reader_free(ts_csv_reader_t *csv_reader);

int ts_csv_reader_read_next(ts_csv_reader_t *csv_reader);

#endif
