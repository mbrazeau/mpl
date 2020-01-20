//
//  mpl_utils.h
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_utils_h
#define mpl_utils_h

#include <stdlib.h>
#include <stdint.h>

typedef void* mpl_str;

#define safe_free(ptr) __safe_free((void*)&ptr)

void* safe_calloc(const unsigned long nelems, const size_t size);
void __safe_free(void** p);
void** alloc_matrix(const size_t nrows, const size_t ncols, const size_t datasz);
mpl_str* mpl_str_new(const long capacity);
void mpl_str_delete(mpl_str** old);
int mpl_str_append(const char c, mpl_str* s);
int mpl_str_append_int(long i, mpl_str* s);
int mpl_str_clear(mpl_str* s);
char* mpl_str_c(mpl_str* s);
uint32_t mpl_rng(void);
int mpl_rng_set_seed(const uint32_t seed);
uint32_t mpl_rng_between(uint32_t min, uint32_t max);
uint32_t mpl_rng_get_seed(void);
int mpl_compchar(const void* int1, const void* int2);
#endif /* mpl_utils_h */
