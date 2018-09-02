#include "ts_list.h"
#include "ts_misc.h"

ts_list_elem_t *ts_list_elem_new(void *value, const size_t size)
{
	ts_list_elem_t *elem=0;

	elem = (ts_list_elem_t *)calloc(1, sizeof(ts_list_elem_t));
	if(0==elem) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	elem->size=size;
	if(0==elem->size) {
		elem->value=value;    
	} else {
		elem->value = (void *)calloc(1, size);
		if(0==elem->value) {
			(void)ts_warn(stderr, "\n");
			goto fail;
		}
		(void *)memcpy(elem->value, value, size);
	}
  
	return (elem);

fail:
	if(elem) {
		(void)ts_list_elem_free(elem);
	}
	return(0);
}

int ts_list_elem_free(ts_list_elem_t *elem)
{
	if(0==elem) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	if(elem->size) {
		if(elem->value) {
			free(elem->value);
		}
	}
	elem->value=0;
	elem->next=0;
	free(elem);

	return(1);

fail:
	return(0);
}

unsigned ts_list_count(const ts_list_elem_t* elem)
{
	unsigned ret = 0;
	for(; elem; elem = elem->next)
		ret ++;
	return ret;
}

int ts_list_append_elem(ts_list_elem_t **head, ts_list_elem_t *elem)
{
	ts_list_elem_t *tail=0;
  
	if(0==head) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}

	if(0==*head) {
		*head=elem;
		return(1);
	} else {
		tail = *head;
		while(tail->next) {
			tail=tail->next;
		}
		tail->next=elem;
	}

	return (1);

fail:
	return(0);
}

int ts_list_append(ts_list_elem_t **head, void *value, const size_t sz)
{
	ts_list_elem_t *elem=0;
	int rv;

	elem=ts_list_elem_new(value, sz);
	if(0==elem) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}

	rv=ts_list_append_elem(head, elem);
	if(0==rv) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}

	return(1);

fail:
	return(0);
}

int ts_list_prepend_elem(ts_list_elem_t **head, ts_list_elem_t *elem)
{
	if(0==head || 0==elem) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	elem->next = *head;
	*head=elem;

	return (1);

fail:
	return(0);
}

int ts_list_prepend(ts_list_elem_t **head, void *value, const size_t sz)
{
	ts_list_elem_t *elem=0;
	int rv;

	elem=ts_list_elem_new(value, sz);
	if(0==elem) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}

	rv=ts_list_prepend_elem(head, elem);
	if(0==rv) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	return(1);

fail:
	return(0);
}

int ts_list_insert_elem(ts_list_elem_t **after, ts_list_elem_t *elem)
{
	if(0==after || 0==elem) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	if(*after) {
		elem->next = (*after)->next;
		(*after)->next = elem;
	} else {
		*after=elem;
	}
  
	return (1);

fail:
	return(0);
}

int ts_list_insert(ts_list_elem_t **after, void *value, size_t sz)
{
	ts_list_elem_t *elem=0;

	elem=ts_list_elem_new(value, sz);
	if(0==after || 0==elem) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	if(*after) {
		elem->next = (*after)->next;
		(*after)->next = elem;
	} else {
		*after=elem;
	}
  
	return (1);
  
fail:
	return(0);
}

ts_list_elem_t *ts_list_search_elem(ts_list_elem_t **head, const void *target, ts_list_compare_t cmpfunc)
{
	ts_list_elem_t *elem=0;
	ts_list_elem_t *pelem=0;

	if(0==head || 0==*head) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	elem = *head;
	if(0==cmpfunc) {
		while(elem) {
			if(elem->value == target) {
				return(elem);
			}
			pelem=elem;
			elem=elem->next;
		}
	} else {
		while(elem) {
			if(0==(*cmpfunc)(elem->value, target)) {
				return(elem);
			}
			pelem=elem;
			elem=elem->next;
		}
	}
  
	return(0);
  
fail:
	return(0);
}


int ts_list_remove(ts_list_elem_t **head, const void *target, ts_list_compare_t cmpfunc)
{
	ts_list_elem_t *elem=0;
	ts_list_elem_t *pelem=0;
	int rv;

	if(0==head || 0==*head) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	elem = *head;
	if(0==cmpfunc) {
		while(elem) {
			if(elem->value == target) {
				if(0==pelem) { // header
					*head = elem->next;
				} else {
					pelem->next=elem->next;
					elem->next=0;
				}
				rv=ts_list_elem_free(elem);
				if(0==rv) {
					(void)ts_warn(stderr, "\n");
					goto fail;
				}
				return(1);
			}
			pelem=elem;
			elem=elem->next;
		}
	} else {
		while(elem) {
			if(0==(*cmpfunc)(elem->value, target)) {
				if(0==pelem) { // header
					*head = elem->next;
				} else {
					pelem->next=elem->next;
					elem->next=0;
				}
				rv=ts_list_elem_free(elem);
				if(0==rv) {
					(void)ts_warn(stderr, "\n");
					goto fail;
				}
				return(1);
			}
			pelem=elem;
			elem=elem->next;
		}
	}

	return(0);

fail:
	return(0);
}

void *ts_list_pop(ts_list_elem_t **head)
{
	ts_list_elem_t *elem=0;
	void *value=0;

	if(0==head || 0==*head) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
	elem=*head;
	value=elem->value;
	*head=elem->next;
	free(elem);
	return(value);

fail:
	return(0);
}

unsigned int ts_list_write(ts_list_elem_t **head, unsigned int(*writer)(void **, FILE *), FILE *fp)
{
	unsigned int tsize=0;
	unsigned int size=0;
	ts_list_elem_t *celem=0;
	int hasMore=1;
  
	if(0 == head || 0 == *head) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	celem = *head;
	while(celem) { /*write, size first, its an int, and then field, and then hasMore */
		size=fwrite(&celem->size, sizeof(int), 1, fp);
		if(1 != size)
			return(0);
		tsize += size;
    
		size=(*writer)(&(celem->value), fp);
		if(0==size) {
			return(0);
		}
		tsize += size;
    
		if(celem->next) {
			hasMore=1;
		} else {
			hasMore=0;
		}

		size=fwrite(&hasMore, sizeof(int), 1, fp);
		if(1 != size) {
			return(0);
		}
		tsize += size;

		celem=celem->next;
	}
  
	return(tsize);
  
fail:
	return(0);
}


unsigned int ts_list_read(ts_list_elem_t **head, unsigned int (*reader)(void **, FILE *), FILE *fp)
{
	unsigned int size=0;
	unsigned int tsize=0;
	unsigned int *sizeptr=&size;
	ts_list_elem_t *celem=0;
	ts_list_elem_t *nelem=0;
	int hasMore=0;
	int *hasMorePtr=&hasMore;
	int rv;

	if(0==head || 0==fp) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	if(!feof(fp)) {
		if(*head) {
			rv=ts_list_elem_free(*head);
			*head=0;
		}
		*head=(ts_list_elem_t *)calloc(1, sizeof(ts_list_elem_t));
		if(NULL == *head) {
			(void)ts_warn(stderr, "\n");
			goto fail;
		}      
    
		if(1!=fread((int *)sizeptr, sizeof(int), 1, fp)) {
			(void)ts_warn(stderr, "\n");
			goto fail;
		}
		(*head)->size=size;
		tsize += size;

		size=(*reader)(&((*head)->value), fp);
		if(0==size || NULL==(*head)->value) {
			(void)ts_warn(stderr, "\n");
			goto fail;      
		}
		tsize += size;
    
		size=fread((void *)hasMorePtr, sizeof(int), 1, fp);
		if(1!=size) {
			(void)ts_warn(stderr, "\n");
			goto fail;
		}
		tsize += size;
    
		celem = *head;
		while(hasMore && !feof(fp)) {
			nelem=(ts_list_elem_t *)calloc(1, sizeof(ts_list_elem_t));
			if(NULL==nelem) {
				(void)ts_warn(stderr, "\n");
				goto fail;
			}
      
			size=fread((int *)sizeptr, sizeof(int), 1, fp);
			if(1!=size) {
				(void)ts_warn(stderr, "\n");
				goto fail;
			}
			tsize += size;
			nelem->size=size;
      
			size=(*reader)(&(nelem->value), fp);
			if(0==size || NULL==nelem->value) {
				(void)ts_warn(stderr, "\n");
				goto fail;
			}
			tsize += size;

			size=fread((void *)hasMorePtr, sizeof(int), 1, fp);
			if(1!=size) {
				(void)ts_warn(stderr, "\n");
				goto fail;
			}
			tsize += size;
			celem->next=nelem;
			celem=nelem;
		}
	}
  
	return(tsize);
  
fail:
	return(0);
}

int ts_list_map(ts_list_elem_t **head, int (*execfunc)(void *))
{
	ts_list_elem_t *elem=0;
	int rv;
  
	if(0==head || 0==*head) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	elem = *head;
	while(elem) {
		rv=(*execfunc)(elem->value);
		if(0==rv) {
			(void)ts_warn(stderr, "\n");
			goto fail;
		}
		elem=elem->next;
	}
  
	return(1);

fail:
	return(0);
}

int ts_list_free(ts_list_elem_t **head)
{
	ts_list_elem_t *celem=0;
	ts_list_elem_t *pelem=0;
	int rv;

	if(0==head || 0==*head) {
		(void)ts_warn(stderr, "\n");
		goto fail;
	}
  
	celem = *head;
	while(celem) {
		pelem=celem;
		celem=pelem->next;
		rv=ts_list_elem_free(pelem);
		if(0==rv) {
			(void)ts_warn(stderr, "\n");
			goto fail;
		}
	}
	*head=0;
  
	return(1);

fail:
	return(0);
}
