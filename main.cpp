#include <malloc.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <time.h>

#include "config.hpp"
#include "chainhashtable.hpp"


union num {

    int i;
    unsigned int ui;
    float f;
};


void testHashTable (const char *out_file_name);

unsigned int hashDiv  (const void *unit);
unsigned int hashUint (const void *unit);
unsigned int hashMul  (const void *unit);
unsigned int hashInt  (const void *unit);

unsigned int hashLen     (const void *unit);
unsigned int hashSum     (const void *unit);
unsigned int hashPolinom (const void *unit);
unsigned int hashCrc32   (const void *unit);


int main () {

    srand((unsigned int) time(NULL));

    testHashTable("hashtestNormal.txt"); 

    return 0;
}

void testHashTable (const char *out_file_name) {

    FILE *out = fopen(out_file_name, "w");
    fclose(out);

    for (int i = 10000; i <= 1000000; i += 10000) {

        out = fopen(out_file_name, "a");

        int *operation = (int*) calloc(i, sizeof(int));
        int *data      = (int*) calloc(i, sizeof(int));

        HashTable ht = hashTableCtor(i, sizeof(int), hashDiv);

        for (int j = 0; j < i; j++) {

            operation[j] = rand() % 3;
            data[j]      = rand();
        }

        clock_t time_start = clock();
        for (int j = 0; j < i; j++) {

            switch (operation[j]) {

                case 0:
                    
                    hashTableInsert(&data[j], &ht);

                    break;

                case 1:
                    
                    hashTableDel(&data[j], &ht);

                    break;

                case 2:
                    
                    hashTableFind(&data[j], &ht);

                    break;
                
                default:
                    break;
            }
        }
        clock_t time_end = clock();

        fprintf(out, "%d %lg\n", i, 1000.0 * ((double) (time_end - time_start)) / CLOCKS_PER_SEC);

        hashTableDtor(&ht);
        free(operation);
        free(data);
        fclose(out);
    }
}

unsigned int hashDiv (const void *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, 0);

    return (*((const int*) unit)) % 1000;
}

unsigned int hashUint (const void *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, 0);

    return ((const num*) unit)->ui;
}

unsigned int hashMul (const void *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, 0);

    double hash = (double) ((const num*) unit)->i * 0.618;

    return (unsigned int) ((hash - (int) hash) * 1000);
}

unsigned int hashInt (const void *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, 0);

    return ((const num*) unit)->i;
}

unsigned int hashLen (const void *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, 0);

    return (unsigned int) strlen(*((char* const*) unit));
}

unsigned int hashSum (const void *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, 0);

    char *str = *((char*const *) unit); 
    unsigned int sum = 0;
    size_t len = strlen(str); 

    for (size_t i = 0; i < len; i++) {

        sum += str[i];
    }

    return sum;
}

unsigned int hashPolinom (const void *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, 0);

    char *str = *((char*const *) unit); 
    unsigned int sum = 0;
    size_t len = strlen(str); 

    for (size_t i = 0; i < len; i++) {

        sum *= 13;
        sum += str[i];
    }

    return sum;
}

unsigned int hashCrc32 (const void *unit) {

    RET_ON_VAL(!unit, ERR_INV_ARG, 0);

    char *str = *((char*const *) unit); 
    size_t len = strlen(str); 

    unsigned crc_table[256];
    unsigned crc;

    for (int i = 0; i < 256; i++){
        crc = i;
        for (int j = 0; j < 8; j++) {

            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        }

        crc_table[i] = crc;
    };

    crc = 0xFFFFFFFFUL;

    while (len--)
        crc = crc_table[(crc ^ *str++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}
