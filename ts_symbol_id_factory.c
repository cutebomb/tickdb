#include "ts_symbol_id_factory.h"
#include "ts_misc.h"
#include <assert.h>

int _ts_symbol_id_factory_realloc(ts_symbol_id_factory_t *idf);
int ts_symbol_id_factory_reload(ts_symbol_id_factory_t *idf);

ts_symbol_id_factory_t *ts_symbol_id_factory_new(unsigned int size, const char *file, enum ts_symbol_id_factory_mode mode)
{
  ts_symbol_id_factory_t *idf=0;

  int rv;
  char* modeOpenFile = NULL;


  if(size<1) {
    size=1;
  }

  idf=(ts_symbol_id_factory_t *)calloc(1, sizeof(ts_symbol_id_factory_t));
  if(0==idf) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  idf->id_map=ts_hash_table_new(size, sizeof(unsigned int));
  if(0==idf->id_map) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  idf->mentry=idf->id_map->mentry;
  idf->nentry=0;

  idf->symbol_ar=(char **)calloc(idf->mentry, sizeof(char *));
  if(0==idf->symbol_ar) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  idf->mode = mode;

  switch(idf->mode) {
  case TS_SYMBOL_ID_FACTORY_MODE_READ:
    modeOpenFile = "r";
    break;
  case  TS_SYMBOL_ID_FACTORY_MODE_WRITE:
    modeOpenFile = "w+";
    break;
  case TS_SYMBOL_ID_FACTORY_MODE_APPEND:
    modeOpenFile = "a+";
    break;
  case TS_SYMBOL_ID_FACTORY_MODE_NONE:
    break;
  }

  if(file && *file) {
    idf->fname=strdup(file);
    if(0==idf->fname) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    if(0==modeOpenFile|| 0==*modeOpenFile) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    idf->fp=fopen(idf->fname, modeOpenFile);
    if(0==idf->fp) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
    idf->fd=fileno(idf->fp);
    rewind(idf->fp);
    rv=ts_symbol_id_factory_reload(idf);
    if(0==rv) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
  }

  return(idf);

 fail:
  if(idf) {
    (void)ts_symbol_id_factory_free(idf);
  }
  return(0);
}

int ts_symbol_id_factory_add(ts_symbol_id_factory_t *idf, char *symbol)
{
  char wbuf[BUFSIZ];
  int size;
  unsigned int *id_ptr=0;
  int rv;

  if(0 == idf || 0 == symbol || '\0' == *symbol) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  id_ptr=(unsigned int *)ts_hash_table_get(idf->id_map, symbol);
  if(id_ptr) {
    (void)fprintf(stderr, "Symbol %s had id %i, adding ignored\n", symbol, *id_ptr);
    return(1);
  }

  if(idf->nentry >= idf->mentry-1) {
    idf->symbol_ar=(char **)realloc(idf->symbol_ar, (1+2*idf->mentry)*sizeof(char *));
    if(0==idf->symbol_ar) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
    (void)memset(idf->symbol_ar+idf->mentry, 0, (1+idf->mentry)*sizeof(char *));
    idf->mentry = 1+2*idf->mentry;
  }

  rv=ts_hash_table_insert(idf->id_map, symbol, &(idf->nentry));
  if(0==rv) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  if(idf->symbol_ar[idf->nentry]) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  idf->symbol_ar[idf->nentry]=strdup(symbol);
  if(0==idf->symbol_ar[idf->nentry]) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  if(idf->mode == TS_SYMBOL_ID_FACTORY_MODE_WRITE || idf->mode == TS_SYMBOL_ID_FACTORY_MODE_APPEND) {
    size=sprintf(wbuf, "%u %s\n", idf->nentry, idf->symbol_ar[idf->nentry]);
    if(size) {
      (void)fwrite(wbuf, sizeof(char), size, idf->fp);
      (void)fflush(idf->fp);
    }
  }

  idf->nentry++;

  return(1);

 fail:
  return(0);
}

unsigned int *ts_symbol_id_factory_get_id(ts_symbol_id_factory_t *idf, char *symbol)
{
  unsigned int *id_ptr=0;
  int rv;

  if(0==idf || 0==symbol || '\0' == *symbol) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  id_ptr=(unsigned int *)ts_hash_table_get(idf->id_map, symbol);
  if(0==id_ptr && idf->fp && idf->mode == TS_SYMBOL_ID_FACTORY_MODE_READ) {
    rv=ts_symbol_id_factory_reload(idf);
    if(0==rv) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
    id_ptr=(unsigned int *)ts_hash_table_get(idf->id_map, symbol);
  }

  return(id_ptr);

 fail:
  return(0);
}

unsigned ts_symbol_id_factory_retrieve_id(ts_symbol_id_factory_t *idf, char *symbol)
{
  unsigned *idp = 0;

  assert(idf);
  assert(symbol);

  idp = ts_symbol_id_factory_get_id(idf, symbol);
  if(! idp) {
    if(! ts_symbol_id_factory_add(idf, symbol)) {
      (void) ts_warn(stderr, "\n");
      goto fail;
    }

    idp = ts_symbol_id_factory_get_id(idf, symbol);
    if(! idp) {
      (void) ts_warn(stderr, "\n");
      goto fail;
    }
  }

  return *idp;
 fail:
  return UINT_MAX;
}

int ts_symbol_id_factory_update_id(ts_symbol_id_factory_t *idf, char *symbol, unsigned int id)
{
  unsigned int *id_ptr=0;
  char wbuf[BUFSIZ];
  int size;
  int rv;

  if(0==idf || 0==symbol || '\0' == *symbol) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  id_ptr=(unsigned int *)ts_hash_table_get(idf->id_map, symbol);

  if(0==id_ptr) {
    if(id>=idf->nentry) {
      idf->nentry=id+1;
    }

    if(idf->nentry >= idf->mentry-1) {
      unsigned m = 8 + 1.2 * idf->nentry;
      idf->symbol_ar=(char **)realloc(idf->symbol_ar, m*sizeof(char *));
      if(0==idf->symbol_ar) {
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
      (void)memset(idf->symbol_ar+idf->mentry, 0, (m-idf->mentry)*sizeof(char *));
      idf->mentry = m;
    }

    rv=ts_hash_table_insert(idf->id_map, symbol, &id);
    if(0==rv) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    if(idf->symbol_ar[id]) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    idf->symbol_ar[id]=strdup(symbol);
    if(0==idf->symbol_ar[id]) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    if(idf->mode == TS_SYMBOL_ID_FACTORY_MODE_WRITE || idf->mode == TS_SYMBOL_ID_FACTORY_MODE_APPEND) {
      size=sprintf(wbuf, "%u %s\n", id, symbol);
      if(size) {
	(void)fwrite(wbuf, sizeof(char), size, idf->fp);
	(void)fflush(idf->fp);
      }
    }
  } else {
    if(id != *id_ptr) {
      (void)ts_warn(stderr, "conflicting id %s old_id %u new_id %u\n", symbol, *id_ptr, id);
      return(0);
    }
  }

  return(1);

 fail:
  return(0);
}

char *ts_symbol_id_factory_get_symbol(ts_symbol_id_factory_t *idf, unsigned int id)
{
  int rv;

  if(0==idf) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  if(id>=idf->nentry || 0==idf->symbol_ar[id]) {
    if(idf->fp && idf->mode == TS_SYMBOL_ID_FACTORY_MODE_READ) {
      rv=ts_symbol_id_factory_reload(idf);
      if(0==rv) {
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
    }
  }

  if(id<=idf->nentry) {
    return(idf->symbol_ar[id]);
  }

  return(0);

 fail:
  return(0);
}

int ts_symbol_id_factory_reload(ts_symbol_id_factory_t *idf)
{
  char buf[BUFSIZ];
  char *s=0;
  char *end;
  char *line=0;
  char *rline=0;
  char *idstr=0;
  char *symbol=0;
  unsigned int id;
  unsigned int *id_ptr;
  int rv;

  if(0==idf || 0==idf->fp) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  (void)memset(buf, 0, BUFSIZ*sizeof(char));
  while(1) {
    s=fgets(buf, BUFSIZ, idf->fp);
    if(0==s) {
      break;
    }
    end=strchr(buf, '\n');
    if(0==end) {
      end=buf+strlen(buf);
    }
    if(end) {
      while(end>=buf && (' ' == *end || '\t' == *end || '\n' == *end)) {
	*end='\0';
	end--;
      }
    }
    if('\0' == *buf) {
      continue;
    }
    line=buf;

    idstr=strtok_r(line, " ", &symbol);
    if(0==idstr) {
      continue;
    }

    id=strtoul(idstr, &rline, 10);

    if(0==symbol || '\0'==*symbol) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    (void) ts_string_trim(symbol);

    id_ptr=(unsigned int *)ts_hash_table_get(idf->id_map, symbol);
    if(id_ptr) {
      if(id == *id_ptr) {
	(void)fprintf(stderr, "Duplicated Entry %u %s\n", id, symbol);
	continue;
      } else {
	(void)ts_warn(stderr, "Conflicting Entry %u %s %u\n", id, symbol, *id_ptr);
	goto fail;
      }
    }

    if(id>=idf->nentry) {
      idf->nentry = id+1;
    }

    if(idf->nentry >= idf->mentry-1) {
      int mentry=max(idf->nentry, 1+2*idf->mentry);
      idf->symbol_ar=(char **)realloc(idf->symbol_ar, mentry*sizeof(char *));
      if(0==idf->symbol_ar) {
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
      (void)memset(idf->symbol_ar+idf->mentry, 0, (mentry - idf->mentry)*sizeof(char *));
      idf->mentry = mentry;
    }

    rv=ts_hash_table_insert(idf->id_map, symbol, &(id));
    if(0==rv) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    if(idf->symbol_ar[id]) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    idf->symbol_ar[id]=strdup(symbol);
    if(0==idf->symbol_ar[id]) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }

    (void)memset(buf, 0, BUFSIZ*sizeof(char));
  }

  return(1);

 fail:
  return(0);
}

int ts_symbol_id_factory_save(ts_symbol_id_factory_t *idf, FILE *fp)
{
  unsigned int id;

  if(0==idf) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  if(0==fp) {
    fp=stdout;
  }

  //if(idf->fname) {
  //	(void)fprintf(fp, "# %s\n", idf->fname);
  //}

  for(id=0; id<idf->nentry; id++) {
    if(idf->symbol_ar[id]) {
      (void)fprintf(fp, "%u %s\n", id, idf->symbol_ar[id]);
    }
  }

  return(1);

 fail:
  return(0);
}

int ts_symbol_id_factory_free(ts_symbol_id_factory_t *idf)
{
  int rv;

  if(0==idf) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  if(idf->symbol_ar) {
    unsigned int i;

    for(i=0; i<idf->mentry; i++) {
      if(idf->symbol_ar[i]) {
	free(idf->symbol_ar[i]);
	idf->symbol_ar[i]=0;
      }
    }
    free(idf->symbol_ar);
    idf->symbol_ar=0;
  }

  if(idf->id_map) {
    rv=ts_hash_table_free(idf->id_map);
    if(0==rv) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
    idf->id_map=0;
  }

  if(idf->fp) {
    fclose(idf->fp);
    idf->fp=0;
  }

  if(idf->fname) {
    free(idf->fname);
    idf->fname=0;
  }

  free(idf);

  return(1);

 fail:
  return(0);
}

ts_symbol_id_factory_t *ts_symbol_id_factory_load(char *sfile, char *file, enum ts_symbol_id_factory_mode mode)
{
  ts_symbol_id_factory_t *idf=0;
  FILE *fp=0;
  char buf[BUFSIZ];
  int rv;

  idf=ts_symbol_id_factory_new(0, file, mode);
  if(0==idf) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  fp=fopen(sfile, "r");
  if(0==fp) {
    (void)ts_warn(stderr, "Can't open input file %s\n", sfile);
    goto fail;
  }

  (void)memset(buf, 0, BUFSIZ*sizeof(char));
  while(fgets(buf, BUFSIZ, fp)) {
    char *beg=0;
    char *end=0;

    end=strchr(buf, '\n');
    if(0==end) {
      end=buf+strlen(buf);
    }

    if(end==buf) {
      continue;
    }

    if(end) {
      while(end>=buf && (' ' == *end || '\t' == *end || '\n' == *end)) {
	*end='\0';
	end--;
      }
    }

    if('\0'==*buf) {
      continue;
    }

    beg=buf;
    while(beg<end && (' ' == *beg || '\t' == *beg || '\n' == *beg)) {
      beg++;
    }
    if(0==*beg) {
      continue;
    }
    rv=ts_symbol_id_factory_add(idf, beg);
    if(0==rv) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
    (void)memset(buf, 0, BUFSIZ*sizeof(char));
  }
  fclose(fp);

  return(idf);

 fail:
  return(0);
}
