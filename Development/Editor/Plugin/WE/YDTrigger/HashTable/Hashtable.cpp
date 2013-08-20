#include "hashtable.h"
#include "../Storm/StormAdapter.h"
#include <stdlib.h>

#define GETENTRY(t, hash) ((t)->table[(hash) % (t)->buckets])

unsigned int
get_hash_code(const char* name, unsigned int len)
{
  register unsigned char* p = (unsigned char *) name;
  register unsigned int l = len;
  register unsigned int h = len;

  while (l-- > 0)
    h = h ^ ((h<<5) + (h>>2) + *p++);

  return h;
}

static hashentry*
table_new_entry(hashtable* h, const char* name, unsigned int len, unsigned int value, unsigned int hash)
{
  hashentry* entry = (hashentry*)malloc(sizeof(hashentry));
  entry->name = name;
  entry->value = value;
  entry->hash = hash;
  entry->next = GETENTRY(h, hash);
  GETENTRY(h, hash) = entry;
  return entry;
}

hashentry*
hashtable_put(hashtable* h, const char* name, unsigned int value)
{
  int len  = BLZSStrLen(name);
  unsigned int hash = get_hash_code(name, len);
  hashentry* entry;
  for (entry = GETENTRY(h, hash); entry != NULL; entry = entry->next)
  {
    if ((entry->hash == hash) && (BLZSStrLen(entry->name) == len) && (0 == BLZSStrCmp(entry->name, name, len)))
    {
      return entry;
    }
  }

  return table_new_entry(h, name, len, value, hash);
}

hashentry*
hashtable_get(hashtable* h, const char* name, unsigned int* pvalue)
{
  unsigned int len  = BLZSStrLen(name);
  unsigned int hash = get_hash_code(name, len);
  hashentry* entry;
  
  for (entry = GETENTRY(h, hash); entry != NULL; entry = entry->next)
  {
    if ((entry->hash == hash) && (BLZSStrLen(entry->name) == len) && (0 == BLZSStrCmp(entry->name, name, len)))
    {
      *pvalue = entry->value;
      return entry;
    }
  }
  
  *pvalue = 0xFFFFFFFF;
  return NULL;
}

hashtable*
hashtable_new(unsigned int hashsize)
{
  hashtable* h = (hashtable*)malloc(sizeof(hashtable));
  h->buckets = hashsize;
  h->table   = (hashentry**)malloc(hashsize * sizeof(hashentry*));
  memset(h->table, 0, hashsize * sizeof(hashentry*));

  return h;
}

void
hashtable_delete(hashtable* h)
{
  hashentry** pentry;
  hashentry*  entry;
  hashentry*  temp;
  for (pentry = h->table; pentry < &h->table[h->buckets]; pentry++)
  {
    for (entry = *pentry; entry  != NULL;)
	  {
	    temp = entry;
	    entry = entry->next;
	    free(temp);
	  }
  }

  free(h->table);
  free(h);
}
