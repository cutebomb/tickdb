#include "ts_csv_reader.h"

ts_csv_reader_t *ts_csv_reader_new(const char *filename)
{
  ts_csv_reader_t *csv_reader = (ts_csv_reader_t*)calloc(1, sizeof(ts_csv_reader_t));
  TS_TRY(csv_reader);
  TS_TRY(csv_reader->file = ts_fopen(filename, "r"));
  
  char buf[BUFSIZ];
  TS_TRY(NULL !=ts_fgets(buf, BUFSIZ, csv_reader->file));
  buf[strlen(buf)-1] = '\0';
  int i=0;
  char *str, *saveptr;
  for(str = buf;;str=NULL) {
    if(strtok_r(str, ",", &saveptr)) {
      i++;
    } else {
      break;
    }
  }
  TS_TRY(0 == ts_fseek(csv_reader->file, 0, SEEK_SET));
  csv_reader->ncol = i;
  csv_reader->elem_ar = (char**)calloc(csv_reader->ncol, sizeof(char*));
  for(i=0;i<csv_reader->ncol;i++) {
    csv_reader->elem_ar[i] = (char*)calloc(ELEM_LEN, sizeof(char));
  }

  csv_reader->head_ar = (char**)calloc(csv_reader->ncol, sizeof(char*));
  for(i=0;i<csv_reader->ncol;i++) {
    csv_reader->head_ar[i] = (char*)calloc(ELEM_LEN, sizeof(char));
  }

  return csv_reader;
 fail:
  return 0;
}
int ts_csv_reader_free(ts_csv_reader_t *csv_reader)
{
  if(csv_reader->file) {
    ts_fclose(csv_reader->file);
    csv_reader->file = 0;
  }
  TS_TRY(csv_reader);
  return 1;
 fail:
  return 0;
}

int ts_csv_reader_read_next(ts_csv_reader_t *csv_reader)
{
  TS_TRY(csv_reader);

  ts_file_t *fp = csv_reader->file;
  char buf[BUFSIZ];
  
  if(!csv_reader->head_readed) {
    TS_TRY(NULL != ts_fgets(buf, BUFSIZ, fp));
    int t = strlen(buf);
    buf[t-1] = '\0';
    char *p = buf, *next = 0;
    int i=0;
    for(i=0;i<csv_reader->ncol;i++) {
      TS_TRY(next = strtok_r(p, ",", &p));
      TS_TRY(strlen(next) < ELEM_LEN);
      strcpy(csv_reader->head_ar[i], next);    
    }
    csv_reader->head_readed = 1;
  }

  char *rv = ts_fgets(buf, BUFSIZ, fp);
  int t = strlen(buf);
  buf[t-1] = '\0';

  if (rv != NULL) {
    char *p = buf, *next = 0;
    int i=0;
    for(i=0;i<csv_reader->ncol;i++) {
      TS_TRY(next = strtok_r(p, ",", &p));
      TS_TRY(strlen(next) < ELEM_LEN);
      strcpy(csv_reader->elem_ar[i], next);    
    }
  } else {
    return -1;
  }

  return 1;
 fail:
  return 0;
}
