#ifndef INCLUDE_HASHTABLE_H
#define INCLUDE_HASHTABLE_H

typedef struct hashentry
{
  const char*       name;
  unsigned int      hash;
  unsigned int      value;
  struct hashentry* next;
}
hashentry;

typedef struct hashtable
{
  unsigned int       buckets;
  struct hashentry** table;
}
hashtable;

hashtable* hashtable_new(unsigned int hashsize);
void       hashtable_delete(hashtable* h);

hashentry* hashtable_get(hashtable* h, const char* name, unsigned int* pvalue);
hashentry* hashtable_put(hashtable* h, const char* name, unsigned int  value);

#endif
