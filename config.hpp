#ifndef _CONFIG_H
#define _CONFIG_H


enum Errors {

    ERR_INV_ARG   = 0,
    ERR_NULL_PTR  = 1,
    ERR_MEM_ALLOC = 2,
};


#define RET_ON_VAL(err_cond, err_code, ret_val) {                                                           \
        if (err_cond) {                                                                                     \
            printf("Error code: %s in file: %s in function %s in line %d\n", #err_code, __FILE__,           \
            __PRETTY_FUNCTION__, __LINE__);                                                                 \
            return ret_val;                                                                                 \
        }                                                                                                   \
}


#endif