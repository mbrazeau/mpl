//
//  mpl_utils.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "mpl_defs.h"

typedef struct _mpl_string {
    size_t len;
    size_t capacity;
    size_t extension_rate;
    char* str;
} mpl_str;

void* safe_calloc(const unsigned long nelems, const size_t size)
{
    void* ret = NULL;
    
    if (nelems == 0 || size == 0) {
        return ret;
    }
    
    assert(ret = calloc(nelems, size));
    
//    if (ret == NULL) {
//        printf("Fatal: memory allocation error!\n");
//        exit(EXIT_FAILURE);
//    }
    
    return ret;
}

void __safe_free(void** p)
{
    if (p != NULL) {
        if (*p != NULL) {
            free(*p);
        }
        *p = NULL;
    }
}

/*
 * PUBLIC STRING FUNCTIONS
 */

mpl_str* mpl_str_new(const long capacity)
{
    mpl_str* newstr = NULL;
    
    if (!(newstr=(mpl_str*)safe_calloc(1, sizeof(mpl_str)))) {
        return NULL;
    }
    
    if ( !(newstr->str = (char*)safe_calloc((capacity+1), sizeof(char))) ) {
        free(newstr);
        return NULL;
    }
    
    newstr->capacity = capacity;
    newstr->extension_rate = 1;
    newstr->len = 0;
    
    return newstr;
}

void mpl_str_delete(void** strhandl)
{
    mpl_str* old = *((mpl_str**)strhandl);
    if (old) {
        if (old->str) {
            free(old->str);
            old->str = NULL;
        }
        
        free(old);
        *strhandl = NULL;
    }
}

int mpl_str_append(const char c, mpl_str* s)
{
    assert(s->capacity != 0);
    if (s->len < s->capacity - 1) {
        s->str[s->len] = c;
        ++s->len;
        s->str[s->len] = '\0';
    }
    else {
        char* rstr = NULL;
        rstr = (char*)realloc(s->str, (s->capacity + s->extension_rate) * sizeof(char));
        if (!rstr) {
            return -1;
        }
        
        s->str = rstr;
        
        s->capacity += s->extension_rate;
        
        s->str[s->len] = c;
        ++s->len;
        s->str[s->len] = '\0';
    }
    
    return 0;
}

int mpl_str_append_int(long i, mpl_str* s)
{
    size_t intlen = snprintf(NULL, 0, "%li", i) + 1;
    char *buf = NULL;
    buf = (char*)safe_calloc(intlen, sizeof(char));
    
    if (!buf) {
        return -1;
    }
    
    snprintf(buf, intlen, "%li", i);
    
    int j = 0;
    do {
        mpl_str_append(buf[j], s);
        ++j;
    } while (buf[j]);
    
    free(buf);
    return 0;
}

int mpl_str_clear(mpl_str* s)
{
    memset(s->str, 0, s->capacity * sizeof(char));
    return 0;
}

char* mpl_str_c(mpl_str* s)
{
    // NOTE: This function violates encapsulation, it could be replaced
    // with a function that receives copied data via a pointer.
    return s->str;
}

unsigned mpl_rng_seed = 3;//MPL_DEFAULT_RSEED;

unsigned mpl_rng(void)
{
    return (mpl_rng_seed = (397204094 * mpl_rng_seed) % ((unsigned)pow(2, 31) - 1));
}

int mpl_rng_set_seed(const int seed)
{
    assert(seed != MPL_RAND_MAX);
    
    mpl_rng_seed = seed;
    
    return 0;
}

int mpl_rng_get_seed(void)
{
    return mpl_rng_seed;
}


inline int almost_equal(const double fl1, const double fl2, const double eps)
{
    if ((fl1 - fl2) < -eps || (fl1 - fl2) > eps) {
        return 1;
    }
    
    return 0;
}


/*
 * PRIVATE STRING FUNCTIONS
 */


