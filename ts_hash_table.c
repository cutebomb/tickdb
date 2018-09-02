#include "ts_hash_table.h"
#include "ts_misc.h"

static const unsigned int primes [] =
  {
    19, 29, 41, 59, 79, 89, 107, 149, 197, 263, 347, 457, 599, 787, 1031,
    1361, 1777, 2333, 3037, 3967, 5167, 6719, 8737, 11369, 14783,
    19219, 24989, 32491, 42257, 54941, 71429, 92861, 120721, 156941,
    204047, 265271, 344857, 448321, 582821, 757693, 985003, 1280519,
    1664681, 2164111, 2813353, 3657361, 4754591, 6180989, 8035301,
    10445899, 13579681, 17653589, 22949669, 29834603, 38784989,
    50420551, 65546729, 85210757, 110774011, 144006217, 187208107,
    243370577, 316381771, 411296309, 534685237, 695090819, 903618083,
    1174703521, 1527114613, 1985248999
  };

static const unsigned int primeTableSize = sizeof(primes)/sizeof(primes[0]);

unsigned int ts_next_prime(unsigned int proposed_size);
unsigned int ts_hash(const char *key, unsigned int prime);
int ts_rehash(ts_hash_table_t **htbl);

typedef struct ts_key_index_pair
{
  char *key;
  int index;
} ts_key_index_pair_t;

ts_key_index_pair_t *ts_key_index_pair_new(const char *key, int idx);
int ts_key_index_pair_free(ts_key_index_pair_t *ki);

ts_key_index_pair_t *ts_key_index_pair_new(const char *key, int idx)
{
  ts_key_index_pair_t *ki=0;

  if(0==key || 0==*key || idx<0) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
	
  ki=(ts_key_index_pair_t *)calloc(1, sizeof(ts_key_index_pair_t));
  if(0==ki) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
	
  ki->key=strdup(key);
  if(0==ki->key) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  ki->index=idx;

  return(ki);

 fail:
  if(ki) {
    (void)ts_key_index_pair_free(ki);
    ki=0;
  }
  return(0);
}

int ts_key_index_pair_free(ts_key_index_pair_t *ki)
{
  if(0==ki) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  if(ki->key) {
    free(ki->key);
    ki->key=0;
  }

  free(ki);

  return(1);

 fail:
  return(0);
}

unsigned int ts_next_prime(unsigned int size)
{
  unsigned int i=0;
  
  for(i=0; i<primeTableSize; i++) {
    if(primes[i] >= size) {
      return(primes[i]);
    }
  }

  return(primes[primeTableSize-1]);
}

unsigned int ts_hash(const char *key, unsigned int prime)
{
  unsigned int hidx=0;
  int c=0;

  while (0!=(c= *key++)) {
    hidx = c + (hidx<<6)+ (hidx <<16) - hidx;
  }

  return (hidx % prime);      
}


ts_hash_table_t *ts_hash_table_new(unsigned int size, int vsize)
{
  ts_hash_table_t *htbl=0;
  
  htbl = (ts_hash_table_t *)calloc(1, sizeof(ts_hash_table_t));
  if (0 == htbl) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  htbl->type=strdup("ts");
  if(0==htbl->type) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  
  htbl->hash=ts_hash;
  htbl->prime=ts_next_prime(size);
  if(htbl->prime<1) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  htbl->vsize=vsize;
  htbl->mentry=2 * htbl->prime;
  htbl->nentry=0;
  
  htbl->key_index_ar=(ts_list_elem_t **)calloc(htbl->prime, sizeof(ts_list_elem_t *));
  if(0==htbl->key_index_ar) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  htbl->val_ar=(void **)calloc(htbl->mentry, sizeof(void *));
  if(0==htbl->val_ar) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  return (htbl);

 fail:
  if(htbl) {
    (void)ts_hash_table_free(htbl);
  }
  return(0);
}

ts_hash_table_t *ts_hash_table_with_new(unsigned int size, int vsize, char *type, ts_hash_function_t hash)
{
  ts_hash_table_t *htbl=0;
  
  htbl = calloc (1, sizeof(*htbl));
  if (0 == htbl) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  htbl->type=strdup(type);
  if(0==htbl->type) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  htbl->hash = hash ? hash : ts_hash;

  htbl->prime=ts_next_prime(size);
  if(htbl->prime<1) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  htbl->vsize=vsize;
  htbl->mentry=2 * htbl->vsize;
  htbl->nentry=0;
  
  htbl->key_index_ar=(ts_list_elem_t **)calloc(htbl->prime, sizeof(ts_list_elem_t *));
  if(0==htbl->key_index_ar) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  htbl->val_ar=(void **)calloc(htbl->mentry, sizeof(void *));
  if(0==htbl->val_ar) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  return (htbl);

 fail:
  if(htbl) {
    (void)ts_hash_table_free(htbl);
  }
  return(0);
}

int ts_hash_table_insert(ts_hash_table_t *htbl, const char *key, void *value)
{
  unsigned int hidx;
  ts_key_index_pair_t *kidx=0;
  ts_list_elem_t  *elem=0;
  int rv;

  if(0==htbl || 0==key || '\0'==*key) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  if(htbl->nentry >= htbl->mentry) {
    rv=ts_rehash(&htbl);
    if(0==rv) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
  }
  
  hidx = (*(htbl->hash))(key, htbl->prime);

  // check duplicates
  elem=htbl->key_index_ar[hidx];
  while(elem) { 
    kidx=(ts_key_index_pair_t *)elem->value;
    if(0==strcmp(kidx->key, key)) {
      (void)ts_warn(stderr, "Duplicate Key %s is ignored\n", key);
      return(1);
    }
    elem=elem->next;
  }

  kidx=ts_key_index_pair_new(key, htbl->nentry);
  if(0==kidx) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
	
  elem=ts_list_elem_new(kidx, 0);
  if(0==elem) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  
  if(htbl->vsize) {
    htbl->val_ar[kidx->index]=(void *)calloc(1, htbl->vsize);
    if(0==htbl->val_ar[kidx->index]) {
      (void)ts_warn(stderr, "\n");
      goto fail;
    }
    (void *)memcpy(htbl->val_ar[kidx->index], value, htbl->vsize);
  } else {
    htbl->val_ar[kidx->index]=value;
  }
  
  rv = ts_list_append_elem(&(htbl->key_index_ar[hidx]), elem);
  if(0==rv) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  htbl->nentry++;
  
  return(1);

 fail:
  return(0);
}

int ts_hash_table_delete(ts_hash_table_t *htbl, const char *key)
{
  int hidx;
  ts_key_index_pair_t *kidx=0;
  ts_list_elem_t  **head=0;  
  ts_list_elem_t  *celem=0;  
  ts_list_elem_t  *pelem=0;  

  hidx=htbl->hash(key, htbl->prime);
  head=&(htbl->key_index_ar[hidx]);
  celem=*head;
  while(celem) {
    if(0==strcmp(((ts_key_index_pair_t *)celem->value)->key, key)) {
      kidx=(ts_key_index_pair_t *)celem->value;
      if(htbl->vsize) {
	// we manage the memory
	if(0==htbl->val_ar[kidx->index]) { 
	  (void)ts_warn(stderr, "\n");
	  goto fail;
	} 
	free(htbl->val_ar[kidx->index]);
      }
      // else client is responsible for freeing his memory
      htbl->val_ar[kidx->index]=0;

      // delete list element
      if(0==pelem) 
	*head=celem->next;
      else 
	pelem->next=celem->next;

      if(0==ts_key_index_pair_free(kidx)) { 
	(void)ts_warn(stderr, "\n");
	goto fail;
      } 				
      if(0==ts_list_elem_free(celem)) { 
	(void)ts_warn(stderr, "\n");
	goto fail;
      } 				
      return(1);
    }
    pelem=celem;
    celem=celem->next;
  }
  
 fail:
  return(0);
}

int ts_rehash(ts_hash_table_t **htbl_ptr)
{
  ts_hash_table_t *htbl;
  unsigned int oprime;
  ts_list_elem_t  **oki_ar=0;
  unsigned int i;
  ts_list_elem_t  *celem=0;
  ts_list_elem_t  *telem=0;
  int hidx;
  int rv;

  
  if(0==htbl_ptr || 0==*htbl_ptr) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  htbl = *htbl_ptr;
  
  oprime=htbl->prime;
  oki_ar=htbl->key_index_ar;

  htbl->prime=ts_next_prime(htbl->prime+1);
  if(oprime==htbl->prime) {
    (void)fprintf(stderr, "Reached Last Prime %i\n", oprime);
    ts_realloc(htbl->val_ar, htbl->mentry, htbl->mentry+1, void*);
    htbl->mentry += htbl->mentry+1;
    return(1);
  }
  
  htbl->key_index_ar=(ts_list_elem_t  **)calloc(htbl->prime, sizeof(ts_list_elem_t *));
  if(0==htbl->key_index_ar) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  htbl->val_ar=(void **)realloc(htbl->val_ar, (2*htbl->prime) * sizeof(void *));
  if(0==htbl->val_ar) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  if(htbl->mentry<2*htbl->prime) {
    (void)memset(&(htbl->val_ar[htbl->mentry]), 0, (2*htbl->prime-htbl->mentry) * sizeof(void *));
  }
  htbl->mentry = 2 * htbl->prime;
	
  for(i=0; i<oprime; i++) {
    if(0==oki_ar[i])
      continue;
    celem=oki_ar[i];
    while(celem) {
      telem=celem;
      celem=celem->next;
      telem->next=0;
      hidx=htbl->hash(((ts_key_index_pair_t *)(telem->value))->key, htbl->prime);
      rv=ts_list_append_elem(&(htbl->key_index_ar[hidx]), telem);
      if(0==rv) {
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
    }
    oki_ar[i]=0;
  }
  if(oki_ar)
    free(oki_ar);
  
  return(1);
  
 fail:
  return(0);
}


void *ts_hash_table_get(const ts_hash_table_t *htbl, const char *key)
{
  int hidx;
  ts_list_elem_t  *celem=0;  

  hidx=htbl->hash(key, htbl->prime);
  celem=htbl->key_index_ar[hidx];
  while(celem) {
    if(0==strcmp(((ts_key_index_pair_t *)celem->value)->key, key)) {
      return(htbl->val_ar[((ts_key_index_pair_t *)celem->value)->index]);
    }
    celem=celem->next;
  }
  
  return(0);
}

int ts_hash_table_free(ts_hash_table_t *htbl)
{
  unsigned int i;
  ts_list_elem_t  *celem=0;
  ts_list_elem_t  *telem=0;
  ts_key_index_pair_t *pair=0;
  int rv;

  if(0==htbl) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }

  for(i=0; i<htbl->prime; i++) {
    if(0==htbl->key_index_ar[i])
      continue;
    celem=htbl->key_index_ar[i];
    while(celem) {
      pair=(ts_key_index_pair_t *)celem->value;
      rv=ts_key_index_pair_free(pair);
      if(0==rv) {
	(void)ts_warn(stderr, "\n");
	goto fail;
      }
      celem->value=0;
      telem=celem;
      celem=celem->next;
      if(telem)
	free(telem);
    }
    htbl->key_index_ar[i]=0; 
  }
  if(htbl->key_index_ar)
    free(htbl->key_index_ar);

  if(htbl->val_ar) {
    if(htbl->vsize) {
      for(i=0; i<htbl->mentry; i++) {
	if(htbl->val_ar[i]) {
	  free(htbl->val_ar[i]);
	  htbl->val_ar[i]=0;
	}
      }
    }
    free(htbl->val_ar);
  }

  if(htbl->type) {
    free(htbl->type);
    htbl->type=0;
  }

  free(htbl);

  return(1);

 fail:
  return(0);
}

int ts_hash_table_get_keys(const ts_hash_table_t *htbl, unsigned int n, char **key_ar)
{
  unsigned int i;
  ts_list_elem_t  *celem=0;
  ts_key_index_pair_t *pair=0;
  unsigned int count=0;

  if(0==htbl || n<htbl->nentry) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  (void)memset(key_ar, 0, n*sizeof(char *));

  count=0;
  for(i=0; i<htbl->prime; i++) {
    if(0==htbl->key_index_ar[i])
      continue;
    celem=htbl->key_index_ar[i];
    while(celem) {
      pair=(ts_key_index_pair_t *)celem->value;
      if(pair && pair->key && *(pair->key)) {
	key_ar[count]=pair->key;
	count++;
      }
      celem=celem->next;
    }
  }
  
  return(count);
  
 fail:
  return(0);
}

int ts_hash_table_get_vals(ts_hash_table_t *htbl, unsigned int n, void **val_ar)
{
  if(0==htbl || 0==htbl->nentry || n<htbl->nentry) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
  
  (void)memset(val_ar, 0, n*sizeof(void *));
  (void)memcpy(val_ar, htbl->val_ar, htbl->nentry * sizeof(void *));

  return(htbl->nentry);

 fail:
  return(0);
}

int ts_hash_table_apply(ts_hash_table_t *htbl, int(*action)(void *val))
{
  unsigned int i=0;
  unsigned int rv=0;
	
  if(0==htbl) {
    (void)ts_warn(stderr, "\n");
    goto fail;
  }
	
  for(i=0; i<htbl->nentry; i++) {
    if(htbl->val_ar[i])
      rv += action(htbl->val_ar[i]);
  }

  return(rv);
	
 fail:
  return(0);
}

