#ifndef _TS_FILE_H_
#define _TS_FILE_H_

#include <stdio.h>
#include <zlib.h>
#include <sys/stat.h>
#include "ts_xzlib.h"

// ZLIB STDIO FILE WRAPPER -- does zlib or stdio version depending on suffix ("file.gz" versus "file")
//
// zlib can open .gz or normal files for READ, but can only open .gz files for WRITE
// so if you want to be able to open normal files for write you have to 

typedef enum ts_file_type
{
	TS_FILE_NUL=0,
	TS_FILE_STD=1,
	TS_FILE_ZLB=2,
	TS_FILE_XZ=3,
} ts_file_type_t;

typedef struct ts_file
{
	ts_file_type_t type;
	union {
	  FILE   *std;
	  gzFile *zlb;
	  XZFILE *xz;
	} fp;
} ts_file_t;

extern ts_file_t *ts_fopen(const char *filepath, const char *mode);
extern int ts_fclose(ts_file_t *fp);
extern int ts_feof(ts_file_t *fp);
extern char *ts_fgets(char *s, int size, ts_file_t *fp);
extern int ts_fseek(ts_file_t *fp, long offset, int whence);
extern ts_file_t *ts_stderr(void);
extern ts_file_t *ts_stdout(void);
extern size_t ts_fread(ts_file_t *fp, char *buf, int len);
extern size_t ts_fwrite(ts_file_t *fp, const char *buf, int len);
extern int ts_fflush(ts_file_t *fp);
#define ts_fprintf(stream, ...) ( (stream)&&(TS_FILE_ZLB==(stream)->type) ? gzprintf((stream)->fp.zlb, __VA_ARGS__) : ((TS_FILE_STD==(stream)->type) ? fprintf((stream)->fp.std, __VA_ARGS__) : (xzprintf((stream)->fp.xz, __VA_ARGS__))) )

int ts_file_is_gz(const char *fname);
int ts_file_is_xz(const char *fname);
int ts_file_size(const char *filename, size_t *size);

#endif//_TS_FILE_H_
