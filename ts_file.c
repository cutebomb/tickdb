#include "ts_misc.h"
#include "ts_file.h"

int ts_file_is_gz(const char *fname)
{
  char *ext=0;

  if(fname && '\0' != *fname) {
    ext=strrchr(fname, '.');
    if(ext) {
      if(0==strncmp(".gz", ext, 3)) {
	return(1);
      }
    }
  }
  return(0);
}

int ts_file_is_xz(const char *fname)
{
  char *ext=0;

  if(fname && '\0' != *fname) {
    ext=strrchr(fname, '.');
    if(ext) {
      if(0==strncmp(".xz", ext, 3)) {
	return(1);
      }
    }
  }
  return(0);
}

ts_file_t *ts_fopen(const char *filepath, const char *mode)
{
  ts_file_t *qfile=0;
  int len=0;

  TS_TRY(filepath); TS_TRY(*filepath); TS_TRY(mode); TS_TRY(*mode);

  if(filepath){
    len=strlen(filepath);
  }

  TS_TRY( qfile=calloc(1, sizeof(ts_file_t)) );

  if( ts_file_is_gz(filepath) ) {
    qfile->type = TS_FILE_ZLB;
  } else if( ts_file_is_xz(filepath) ) {
    qfile->type = TS_FILE_XZ;
  }
  else {
    qfile->type = TS_FILE_STD;
  }

  switch(qfile->type){
  case TS_FILE_STD: TS_TRY( qfile->fp.std =  fopen(filepath, mode)); break;
  case TS_FILE_ZLB: TS_TRY( qfile->fp.zlb = gzopen(filepath, mode)); break;
  case TS_FILE_XZ:  TS_TRY( qfile->fp.xz =  xzopen(filepath, mode)); break;
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  
  return(qfile);
 fail:
  if(qfile) {
    free(qfile);
    qfile=0;
  }
  return(0);
}

int ts_fclose(ts_file_t *fp)
{
  TS_TRY(fp);

  switch(fp->type)
    {
    case TS_FILE_STD: TS_TRY( 0 ==  fclose(fp->fp.std) ); break;
    case TS_FILE_ZLB: TS_TRY( 0 == gzclose(fp->fp.zlb) ); break;
    case TS_FILE_XZ:  TS_TRY( 0 == xzclose(fp->fp.xz)  ); break;
    default:
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

  free(fp); fp=0;
	
  return(0);

 fail:
  return(EOF);
}

int ts_feof(ts_file_t *fp)
{
  TS_TRY(fp);

  switch(fp->type)
    {
    case TS_FILE_STD: return( feof(fp->fp.std));
    case TS_FILE_ZLB: return(gzeof(fp->fp.zlb));
    case TS_FILE_XZ:  return (xzeof(fp->fp.xz));
    default:
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

 fail:
  return(0);
}

char *ts_fgets(char *s, int size, ts_file_t *fp)
{
  TS_TRY(fp);

  switch(fp->type)
    {
    case TS_FILE_STD: return( fgets(s, size, fp->fp.std));
    case TS_FILE_ZLB: return(gzgets(fp->fp.zlb, s, size)); // different arg order
    case TS_FILE_XZ:  return (xzgets(fp->fp.xz, s, size));
    default:
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

 fail:
  return(0);
}


int ts_fseek(ts_file_t *fp, long offset, int whence)
{
  TS_TRY(fp);

  switch(fp->type)
    {
    case TS_FILE_STD: return( fseek(fp->fp.std, offset, whence));
    case TS_FILE_ZLB: return(gzseek(fp->fp.zlb, offset, whence));
    case TS_FILE_XZ:
      (void)ts_warn(stderr, "seek not implement in xz file format\n");
      goto fail;
    default:
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

 fail:
  return(-1);
}

ts_file_t *ts_stderr(void)
{
  static ts_file_t s_ts_stderr;
	
  s_ts_stderr.type    = TS_FILE_STD;
  s_ts_stderr.fp.std  = stderr;

  return(&s_ts_stderr);
}

ts_file_t *ts_stdout(void)
{
  static ts_file_t s_ts_stdout;
	
  s_ts_stdout.type    = TS_FILE_STD;
  s_ts_stdout.fp.std  = stdout;

  return(&s_ts_stdout);
}

size_t ts_fread(ts_file_t *fp, char *buf, int len)
{
  TS_TRY(fp && buf && len > 0);
  switch(fp->type) {
  case TS_FILE_STD: return(fread((void*)buf, 1, len, fp->fp.std));
  case TS_FILE_ZLB: return(gzread(fp->fp.zlb, (void*)buf, len));
  case TS_FILE_XZ:  //return(xzread(fp->fp.xz, (const void*) buf, len));
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
 fail:
  return(-1);
}

size_t ts_fwrite(ts_file_t *fp, const char *buf, int len)
{
  TS_TRY(fp && buf && len > 0);
  switch(fp->type) {
  case TS_FILE_STD: return(fwrite((const void*)buf, 1, len, fp->fp.std));
  case TS_FILE_ZLB: return(gzwrite(fp->fp.zlb, (const void*)buf, len));
  case TS_FILE_XZ:  return(xzwrite(fp->fp.xz, (const void*) buf, len));
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
 fail:
  return(-1);
}

int ts_fflush(ts_file_t *fp)
{
  TS_TRY(fp);
  int rv = -1;
  switch(fp->type) {
  case TS_FILE_STD: return(fflush(fp->fp.std));
  case TS_FILE_ZLB: 
    rv = gzflush(fp->fp.zlb, Z_SYNC_FLUSH);
    if(rv == Z_OK)
      return 0;
    else
      return EOF;
    break;
  case TS_FILE_XZ:  return (xzflush(fp->fp.xz));
  default:
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
 fail:
  return(-1);
}

int ts_file_size(const char *filename, size_t *size)
{
  TS_TRY(filename && size);
  if(ts_file_is_gz(filename)) {
    int isize;
    FILE* fd = fopen(filename, "r");
    TS_TRY(fd);
    fseek(fd, -4, SEEK_END);
    fread(&isize, sizeof(int), 1, fd);
    *size = isize;
    fclose(fd);
  } else if(ts_file_is_xz(filename)) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  } else {
    struct  stat  buf;
    TS_TRY(0 == stat(filename, &buf));
    *size = buf.st_size;
  }
  return 1;
 fail:
  return 0;
}

