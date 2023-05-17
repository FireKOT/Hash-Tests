#ifndef _OPENHASHTABLE_HPP_
#define _OPENHASHTABLE_HPP_


#include <stdio.h>


struct Unit {

    void *val;

    int state;
};

struct HashTable {

    Unit *table;

    unsigned int (*hash) (const void *unit);

    size_t size;
    size_t elem_size;
    size_t factor;

    size_t capacity, capacity_lim;
};


HashTable hashTableCtor   (size_t size, size_t elem_size, unsigned int (*hash) (const void *unit));
void      hashTableDtor   (HashTable *ht);
Unit      unitCtor        (const void *unit, const HashTable *ht);
void      unitDtor        (Unit *unit);
void      hashTableInsert (const void *unit, HashTable *ht);
void      resizeIfNeed    (HashTable *ht);
int       hashTableFind   (const void *unit, HashTable *ht);
void      hashTableDel    (const void *unit, HashTable *ht);


#endif