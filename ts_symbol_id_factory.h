#ifndef _TS_SYMBOL_ID_FACTORY
#define _TS_SYMBOL_ID_FACTORY

#include "ts_list.h"
#include "ts_hash_table.h"

// Implemented with flock, may not work well over NFS

enum ts_symbol_id_factory_mode {TS_SYMBOL_ID_FACTORY_MODE_READ, TS_SYMBOL_ID_FACTORY_MODE_WRITE, TS_SYMBOL_ID_FACTORY_MODE_APPEND, TS_SYMBOL_ID_FACTORY_MODE_NONE};

typedef struct ts_symbol_id_factory
{
	char *fname;
	FILE *fp;
	int fd;
	enum ts_symbol_id_factory_mode mode;

	unsigned int mentry;
	unsigned int nentry;
	ts_hash_table_t *id_map;
	char **symbol_ar;
} ts_symbol_id_factory_t;

ts_symbol_id_factory_t *ts_symbol_id_factory_new(unsigned int size, const char *file, enum ts_symbol_id_factory_mode mode);
ts_symbol_id_factory_t *ts_symbol_id_factory_load(char *sfile, char *ifile, enum ts_symbol_id_factory_mode mode);
int ts_symbol_id_factory_add(ts_symbol_id_factory_t *idf, char *symbol);
unsigned int *ts_symbol_id_factory_get_id(ts_symbol_id_factory_t *idf, char *symbol);
// retrieve id: if exists return id; if not, add into factory then return id
unsigned ts_symbol_id_factory_retrieve_id(ts_symbol_id_factory_t *idf, char *symbol);
char *ts_symbol_id_factory_get_symbol(ts_symbol_id_factory_t *idf, unsigned int id);
int ts_symbol_id_factory_update_id(ts_symbol_id_factory_t *idf, char *symbol, unsigned int id);
int ts_symbol_id_factory_save(ts_symbol_id_factory_t *idf, FILE *fp);
int ts_symbol_id_factory_free(ts_symbol_id_factory_t *idf);

#endif
