#include <stdio.h>
#include <malloc.h>
#include <string.h> 

#include "openhashtable.hpp"
#include "config.hpp"


enum UnitStates {

    EMPTY   = 0,
    FILLED  = 1,
    DELETED = 2,
};


HashTable hashTableCtor (size_t size, size_t elem_size, unsigned int (*hash) (const void *unit)) {

    HashTable err {

        .table        = nullptr,
        .hash         = nullptr,
        .size         = 0,
        .elem_size    = 0,
        .factor       = 0,
        .capacity     = 0,
        .capacity_lim = 0,
    };

    RET_ON_VAL(!size || !elem_size, ERR_INV_ARG, err);

    if (size == 1) size = 2;

    Unit *table = (Unit*) calloc(size, sizeof(Unit));
    RET_ON_VAL(!table, ERR_MEM_ALLOC, err);

    for (size_t i = 0; i < size; i++) {

        table[i].state = EMPTY;
    }

    return {

        .table        = table,
        .hash         = hash,
        .size         = size,
        .elem_size    = elem_size,
        .factor       = 2,
        .capacity     = 0,
        .capacity_lim = (size_t) ((double) size * 0.7),
    };
}

void hashTableDtor (HashTable *ht) {

    RET_ON_VAL(!ht, ERR_INV_ARG, );

    for (size_t i = 0; i < ht->size; i++) {

        unitDtor(&ht->table[i]);
    }

    free(ht->table);

    ht->table        = nullptr;
    ht->hash         = nullptr;
    ht->size         = 0;
    ht->elem_size    = 0;
    ht->factor       = 0;
    ht->capacity     = 0;
    ht->capacity_lim = 0;
}

Unit unitCtor (const void *unit, const HashTable *ht) {

    Unit err {

        .val   = nullptr,
        .state = -1,
    };

    RET_ON_VAL(!unit || !ht, ERR_INV_ARG, err);

    Unit new_unit {

        .val   = calloc(1, ht->elem_size),
        .state = FILLED,
    };
    memcpy(new_unit.val, unit, ht->elem_size);

    return new_unit;
}

void unitDtor (Unit *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, );

    free(unit->val);
    
    unit->val   = nullptr;
    unit->state = 0;
}

void hashTableInsert (const void *unit, HashTable *ht) {

    RET_ON_VAL(!unit || !ht, ERR_INV_ARG, );

    size_t id = ht->hash(unit) % ht->size;

    while (ht->table[id].state == FILLED) {

        id = (id + 1) % ht->size;
    }    

    if (ht->table[id].state == DELETED) {

        unitDtor(&ht->table[id]);
    }

    ht->table[id] = unitCtor(unit, ht);
    ht->capacity++;

    resizeIfNeed(ht);
}

void resizeIfNeed (HashTable *ht) {

    RET_ON_VAL(!ht, ERR_INV_ARG, );

    if (ht->capacity <= ht->capacity_lim) return;
    
    HashTable new_ht = hashTableCtor(ht->size * ht->factor, ht->elem_size, ht->hash);

    for (size_t i = 0; i < ht->size; i++) {

        if (ht->table[i].state == FILLED) {

            hashTableInsert(ht->table[i].val, &new_ht);
        }
    }

    hashTableDtor(ht);
    *ht = new_ht;
}

int hashTableFind (const void *unit, HashTable *ht) {

    RET_ON_VAL(!unit || !ht, ERR_INV_ARG, -1);

    size_t id = ht->hash(unit) % ht->size;

    while (ht->table[id].state == DELETED || (ht->table[id].state != EMPTY && memcmp(unit, ht->table[id].val, ht->elem_size))) {

        id = (id + 1) % ht->size;
    }    

    if (ht->table[id].state != EMPTY) return 1;
    else return 0;
}

void hashTableDel (const void *unit, HashTable *ht) {

    RET_ON_VAL(!unit || !ht, ERR_INV_ARG, );

    size_t id = ht->hash(unit) % ht->size;

    while (ht->table[id].state == DELETED || (ht->table[id].state != EMPTY && memcmp(unit, ht->table[id].val, ht->elem_size))) {

        id = (id + 1) % ht->size;
    }    

    if (ht->table[id].state != EMPTY) {

        ht->table[id].state = DELETED;
        memset(ht->table[id].val, 0, ht->elem_size);   
    }
}