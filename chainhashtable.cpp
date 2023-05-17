#include <stdio.h>
#include <malloc.h>
#include <string.h> 

#include "hashtable.hpp"
#include "config.hpp"


HashTable hashTableCtor (size_t size, size_t elem_size, unsigned int (*hash) (const void *unit)) {

    HashTable err {

        .table     = nullptr,
        .hash      = nullptr,
        .size      = 0,
        .elem_size = 0,
    };

    RET_ON_VAL(!size, ERR_INV_ARG, err);

    HashTableNode **table = (HashTableNode**) calloc(size, sizeof(HashTableNode*));
    RET_ON_VAL(!table, ERR_MEM_ALLOC, err);

    return {

        .table     = table,
        .hash      = hash,
        .size      = size,
        .elem_size = elem_size,
    };
}

void hashTableDtor (HashTable *ht) {

    RET_ON_VAL(!ht, ERR_INV_ARG, );

    for (size_t id = 0; id < ht->size; id++) {

        hashTableListDtor(ht, id);
    }

    free(ht->table);
    ht->table = nullptr;

    ht->hash = nullptr;
    ht->size = 0;
}

void hashTableInsert (const void *unit, HashTable *ht) {

    RET_ON_VAL(!unit, ERR_INV_ARG, );

    unsigned int hash = ht->hash(unit);

    hashTableListInsert(unit, ht, hash % ht->size);
}

HashTableNode *hashTableNodeCtor (const void *unit, size_t elem_size) {

    RET_ON_VAL(!unit || !elem_size, ERR_INV_ARG, nullptr);

    HashTableNode *node = (HashTableNode*) calloc(1, sizeof(HashTableNode));
    RET_ON_VAL(!node, ERR_MEM_ALLOC, nullptr);

    node->unit = calloc(1, elem_size);
    RET_ON_VAL(!node->unit, ERR_MEM_ALLOC, nullptr);

    memcpy(node->unit, unit, elem_size);

    node->next = nullptr;

    return node;
}

void hashTableNodeDtor (HashTableNode *node) {

    RET_ON_VAL(!node, ERR_INV_ARG, );

    free(node->unit);
    node->next = nullptr;

    free(node);
}

void hashTableListDtor (HashTable *ht, size_t id) {

    RET_ON_VAL(!ht || ht->size <= id, ERR_INV_ARG, );\
    if (!ht->table[id]) return;

    HashTableNode *cur = ht->table[id], *next = cur->next;

    while (next) {

        hashTableNodeDtor(cur);

        cur  = next;
        next = cur->next;
    }
    hashTableNodeDtor(cur);
}

void hashTableListInsert (const void *unit, HashTable *ht, size_t id) {

    RET_ON_VAL(!unit || !ht || id > ht->size, ERR_INV_ARG, );

    HashTableNode *new_node = hashTableNodeCtor(unit, ht->elem_size);

    new_node->next = ht->table[id];
    ht->table[id] = new_node;
}

int hashTableFind (const void *unit, HashTable *ht) {

    RET_ON_VAL(!unit || !ht, ERR_INV_ARG, -1);

    if (!unit || !ht) return -1;

    size_t id = ht->hash(unit) % ht->size;

    HashTableNode *node = ht->table[id];

    while (node && memcmp(unit, node->unit, ht->elem_size)) {

        node = node->next;
    }

    if (node) return 1;
    return 0;
}

void hashTableDel (const void *unit, HashTable *ht) {

    RET_ON_VAL(!unit || !ht, ERR_INV_ARG, );

    size_t id = ht->hash(unit) % ht->size;

    HashTableNode *node = ht->table[id];
    HashTableNode *prev = ht->table[id];

    while (node && memcmp(unit, node->unit, ht->elem_size)) {

        prev = node;
        node = node->next;
    }

    if (node) {

        if (node != ht->table[id]) {

            prev->next = node->next;
        }
        else ht->table[id] = node->next;

        hashTableNodeDtor(node);
    }
}

void hashTableGetCllsn (const HashTable *ht, const char *out_file_name) {

    RET_ON_VAL(!ht, ERR_INV_ARG, );

    FILE *out = fopen(out_file_name, "w");
    RET_ON_VAL(!out, ERR_INV_ARG, );

    for (size_t i = 0; i < ht->size; i++) {

        size_t size = 0;
        HashTableNode *node = ht->table[i];

        while (node) {

            node = node->next;
            size++;
        }

        fprintf(out, "%ld\n", size);
    }

    fclose(out);
}