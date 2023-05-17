#ifndef _CHAINHASHTABLE_HPP_
#define _CHAINHASHTABLE_HPP_


#include <stdio.h>


struct HashTableNode {

    void *unit;

    HashTableNode *next;
};

struct HashTable {

    HashTableNode **table;

    unsigned int (*hash) (const void *unit);

    size_t size;
    size_t elem_size;
};


HashTable hashTableCtor (size_t size, size_t elem_size, unsigned int (*hash) (const void *unit));
void      hashTableDtor   (HashTable *ht);
void      hashTableInsert (const void *unit, HashTable *ht);

HashTableNode *hashTableNodeCtor   (const void *unit, size_t elem_size);
void           hashTableNodeDtor   (HashTableNode *node);
void           hashTableListDtor   (HashTable *ht, size_t id);
void           hashTableListInsert (const void *unit, HashTable *ht, size_t id);
int            hashTableFind       (const void *unit, HashTable *ht);
void           hashTableDel        (const void *unit, HashTable *ht);

void hashTableGetCllsn (const HashTable *ht, const char *out_file_name);


#endif