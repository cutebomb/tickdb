#ifndef _TS_XZLIB_C
#define _TS_XZLIB_C

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <lzma.h>

#define kBufferSize (1 << 15)

typedef struct xzfile {
/*@only@*/
  uint8_t buf[kBufferSize];	/*!< IO buffer */
  lzma_stream strm;		/*!< LZMA stream */
  /*@dependent@*/
  FILE * fp;
  int encoding;
  int eof;

  int have;
  char outbuf[kBufferSize];
} XZFILE;

XZFILE *xzopen(const char *path, const char *mode);
int xzflush(XZFILE *xzfile);
int xzclose(XZFILE *xzfile);
int xzwrite(XZFILE *xzfile, const void *buf, int len);
int xzeof(XZFILE *xzfile);
char *xzgets(XZFILE *xzfile, char *buf, int len);
int xzprintf(XZFILE *xzfile, const char *format, ...);

#endif
