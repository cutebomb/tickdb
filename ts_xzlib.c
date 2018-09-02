#include "ts_xzlib.h"
#include "ts_misc.h"

static XZFILE *xzopen_internal(const char *path, const char *mode, int fdno, int xz)
	/*@globals fileSystem @*/
	/*@modifies fileSystem @*/
{
    int level = LZMA_PRESET_DEFAULT;
    int encoding = 0;
    FILE *fp;
    XZFILE *xzfile;
    lzma_stream tmp;
    lzma_ret ret;

    for (; *mode != '\0'; mode++) {
	if (*mode == 'w')
	    encoding = 1;
	else if (*mode == 'r')
	    encoding = 0;
	else if (*mode >= '0' && *mode <= '9')
	    level = (int)(*mode - '0');
    }
    if (fdno != -1)
	fp = fdopen(fdno, encoding ? "w" : "r");
    else
	fp = fopen(path, encoding ? "w" : "r");
    if (!fp)
	return NULL;
    xzfile = calloc(1, sizeof(*xzfile));
    if (!xzfile) {
	(void) fclose(fp);
	return NULL;
    }
    xzfile->fp = fp;
    xzfile->encoding = encoding;
    xzfile->eof = 0;
    tmp = (lzma_stream)LZMA_STREAM_INIT;
    xzfile->strm = tmp;
    if (encoding) {
	if (xz) {
	    ret = lzma_easy_encoder(&xzfile->strm, level, LZMA_CHECK_CRC32);
	} else {
	    lzma_options_lzma options;
	    (void) lzma_lzma_preset(&options, level);
	    ret = lzma_alone_encoder(&xzfile->strm, &options);
	}
    } else {
	/* We set the memlimit for decompression to 100MiB which should be
	 * more than enough to be sufficient for level 9 which requires 65 MiB.
	 */
	ret = lzma_auto_decoder(&xzfile->strm, 100<<20, 0);
    }
    if (ret != LZMA_OK) {
	(void) fclose(fp);
	memset(xzfile, 0, sizeof(*xzfile));
	free(xzfile);
	return NULL;
    }
    return xzfile;
}

XZFILE *xzopen(const char *path, const char *mode)
	/*@globals fileSystem @*/
	/*@modifies fileSystem @*/
{
    return xzopen_internal(path, mode, -1, 1);
}

int xzflush(XZFILE *xzfile)
	/*@globals fileSystem @*/
	/*@modifies xzfile, fileSystem @*/
{
    return fflush(xzfile->fp);
}

int xzclose(/*@only@*/ XZFILE *xzfile)
	/*@globals fileSystem @*/
	/*@modifies *xzfile, fileSystem @*/
{
    lzma_ret ret;
    size_t n;
    int rc;

    if (!xzfile)
	return -1;
    if (xzfile->encoding) {
	for (;;) {
	    xzfile->strm.avail_out = kBufferSize;
	    xzfile->strm.next_out = (uint8_t *)xzfile->buf;
	    ret = lzma_code(&xzfile->strm, LZMA_FINISH);
	    if (ret != LZMA_OK && ret != LZMA_STREAM_END)
		return -1;
	    n = kBufferSize - xzfile->strm.avail_out;
	    if (n && fwrite(xzfile->buf, 1, n, xzfile->fp) != n)
		return -1;
	    if (ret == LZMA_STREAM_END)
		break;
	}
    }
    lzma_end(&xzfile->strm);
    rc = fclose(xzfile->fp);
    memset(xzfile, 0, sizeof(*xzfile));
    free(xzfile);
    return rc;
}

static int xzread(XZFILE *xzfile, void *buf, int len)
	/*@globals fileSystem @*/
	/*@modifies xzfile, *buf, fileSystem @*/
{
    lzma_ret ret;
    int eof = 0;

    if (!xzfile || xzfile->encoding)
      return -1;
    if (xzfile->eof)
      return 0;
/*@-temptrans@*/
    xzfile->strm.next_out = buf;
/*@=temptrans@*/
    xzfile->strm.avail_out = len;
    for (;;) {
	if (!xzfile->strm.avail_in) {
	    xzfile->strm.next_in = (uint8_t *)xzfile->buf;
	    xzfile->strm.avail_in = fread(xzfile->buf, 1, kBufferSize, xzfile->fp);
	    if (!xzfile->strm.avail_in)
		eof = 1;
	}
	ret = lzma_code(&xzfile->strm, LZMA_RUN);
	if (ret == LZMA_STREAM_END) {
	    xzfile->eof = 1;
	    return len - xzfile->strm.avail_out;
	}
	if (ret != LZMA_OK)
	    return -1;
	if (!xzfile->strm.avail_out)
	    return len;
	if (eof)
	    return -1;
      }
    /*@notreached@*/
}

int xzeof(XZFILE *xzfile)
{
  if(!xzfile || xzfile->encoding)
    return 0;
  if(xzfile->eof && !xzfile->have)
    return xzfile->eof;
  else
    return 0;
}

int xzwrite(XZFILE *xzfile, const void *buf, int len)
	/*@globals fileSystem @*/
	/*@modifies xzfile, fileSystem @*/
{
    lzma_ret ret;
    size_t n;

    if (!xzfile || !xzfile->encoding)
	return -1;
    if (!len)
	return 0;
/*@-temptrans@*/
    xzfile->strm.next_in = buf;
/*@=temptrans@*/
    xzfile->strm.avail_in = len;
    for (;;) {
	xzfile->strm.next_out = (uint8_t *)xzfile->buf;
	xzfile->strm.avail_out = kBufferSize;
	ret = lzma_code(&xzfile->strm, LZMA_RUN);
	if (ret != LZMA_OK)
	    return -1;
	n = kBufferSize - xzfile->strm.avail_out;
	if (n && fwrite(xzfile->buf, 1, n, xzfile->fp) != n)
	    return -1;
	if (!xzfile->strm.avail_in)
	    return len;
    }
    /*@notreached@*/
}

char *xzgets(XZFILE *xzfile, char *buf, int len)
{
  int left, n;
  char *str;
  char *eol;

  if (xzfile == NULL || buf == NULL || len < 1)
    return NULL;

  if (xzfile->encoding != 0)
    return NULL;
  
   /* copy output bytes up to new line or len - 1, whichever comes first --
       append a terminating zero to the string (we don't check for a zero in
       the contents, let the user worry about that) */
    str = buf;
    left = (unsigned)len - 1;
    if (left) do {
	if(!xzfile->have) {
	  xzfile->have = xzread(xzfile, xzfile->outbuf, kBufferSize);
	}
	if(!xzfile->have) {
	  return 0;
	}

        /* look for end-of-line in current output buffer */
        n = xzfile->have > left ? left : xzfile->have;
        eol = memchr(xzfile->outbuf, '\n', n);
        if (eol != NULL)
            n = (unsigned)(eol - xzfile->outbuf) + 1;

        /* copy through end-of-line, or remainder if not found */
        memcpy(buf, xzfile->outbuf, n);
        xzfile->have -= n;
        left -= n;
        buf += n;

	memmove(xzfile->outbuf, xzfile->outbuf + n, xzfile->have);
    } while (left && eol == NULL);

    /* found end-of-line or out of space -- terminate string and return it */

    buf[0] = 0;
    return str;
}

int xzprintf(XZFILE *xzfile, const char *format, ...)
{
  TS_TRY(xzfile && format);
  va_list vs;
  int bufsize = BUFSIZ;
  char *buf = (char *)calloc(bufsize, sizeof(char));
  va_start(vs, format);
  int rv = vsnprintf(buf, bufsize, format, vs);
  if(rv > bufsize) {
    buf = (char *)realloc(buf, rv+10);
    int rv2 = vsnprintf(buf, rv+10, format, vs);
    TS_TRY(rv2 <= rv + 10);
    rv = rv2;
  }
  va_end(vs);
  
  TS_TRY(rv == xzwrite(xzfile, buf, rv));
  
  free(buf);
  return rv;
 fail:
  return 0;
}
