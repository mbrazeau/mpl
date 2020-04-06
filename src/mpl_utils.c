//
//  mpl_utils.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

/*

 This file contains general-purpose utilities used throughout the morphy library
 including memory managment convencience functions, basic dynamic string struct
 definition and functions, a pseudorandom number generator, the random number
 generator's seed variable, floating point comparison function, and integer
 comparison functions for use by sorting algorithms.
 
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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

typedef struct _culink {
    void* data;
    struct _culink* next;
} culink;

typedef struct _culist {
    size_t  capacity;
    size_t  extension_rate;
    culink* pool;
    culink* head;
    culink* back;
} culist;



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

void** alloc_matrix(const size_t nrows, const size_t ncols, const size_t datasz)
{
    size_t i = 0;
    void** ret = NULL;
    
    ret = (void**)safe_calloc(nrows, sizeof(void*));
    
    for (i = 0; i < nrows; ++i) {
        ret[i] = safe_calloc(ncols, datasz);
    }
    
    return ret;
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

int mpl_strcmp(mpl_str* s1, mpl_str* s2)
{
    return strcmp(s1->str, s2->str);
}

char* mpl_str_c(mpl_str* s)
{
    // NOTE: This function violates encapsulation, it could be replaced
    // with a function that receives copied data via a pointer.
    return s->str;
}

/*
* RANDOM NUMBER GENERATION
*/

uint32_t mpl_rng_seed = MPL_DEFAULT_RSEED;

uint32_t mpl_rng(void)
{
    return (mpl_rng_seed = (397204094 * mpl_rng_seed) % ((uint32_t)pow(2, 31) - 1));
}

int mpl_rng_set_seed(const uint32_t seed)
{
    assert(seed != MPL_RAND_MAX);
    
    mpl_rng_seed = seed;
    
    return 0;
}

uint32_t mpl_rng_between(uint32_t min, uint32_t max)
{
    return min + mpl_rng() / (MPL_RAND_MAX / (max - min + 1) + 1);
}

uint32_t mpl_rng_get_seed(void)
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

int mpl_compchar(const void* int1, const void* int2)
{
    return (*(char*)int1) - (*(char*)int2);
}


/*
 * LINKED LISTS
 */

void culist_delete(culist* list)
{
    culink* p = NULL;
    culink* q = NULL;
    
    p = list->pool;
    while (p != NULL) {
        assert(p->data == NULL);
        q = p->next;
        free(p);
        p = q;
    }
    
    p = list->head;
    while (p != NULL) {
        assert(p->data == NULL);
        q = p->next;
        free(p);
        p = q;
    }
    
    list->pool = NULL;
    list->head = NULL;
    list->back = NULL;
    
    free(list);
}

culist* culist_new(size_t nelems)
{
    culist* newlist = NULL;
    newlist = (culist*)safe_calloc(1, sizeof(culist));
    
    if (newlist != NULL) {
        size_t i = 0;
        culink* n = NULL;
        culink* p = NULL;
        n = (culink*)safe_calloc(1, sizeof(culink));
        p = n;
        for (i = 1; i < nelems; ++i) {
            n->next = (culink*)safe_calloc(1, sizeof(culink));
            if (n->next == NULL) {
                culist_delete(newlist);
                return NULL;
            }
            n = n->next;
        }
        newlist->pool = p;
        
        newlist->head = NULL;
        newlist->back = NULL;
    }
    
    return newlist;
}

int culist_push_back(void* data, culist* list)
{
    int ret = -1;
    culink* nlink = NULL;
    culink* p = NULL;
    
    // Get a new link from the pool
    nlink = list->pool;
    if (nlink != NULL) {
        p = nlink->next;
        list->pool = p;
        
        nlink->next = NULL;
        
        if (list->head == NULL) {
            list->head = nlink;
        } else {
            list->back->next = nlink;
        }
        list->back = nlink;
        
        ret = 0;
    }
    
    return ret;
}

void* culist_get(const size_t i, culist* list)
{
    culink* p = NULL;
    
    int k = 0;
    
    p = list->head;
    while (k < i && p != NULL) {
        p = p->next;
        ++k;
    }
    
    assert(p != NULL);
    
    return p->data;
}

/*
 * PRIVATE STRING FUNCTIONS
 */


