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

typedef void* mpl_str;

void* safe_calloc(const unsigned long nelems, const size_t size);
mpl_str* mpl_str_new(const long capacity);
void mpl_str_delete(mpl_str** old);
int mpl_str_append(const char c, mpl_str* s);
int mpl_str_append_int(long i, mpl_str* s);
int mpl_str_clear(mpl_str* s);
char* mpl_str_c(mpl_str* s);
int mpl_rng(void);
int mpl_rng_set_seed(const int seed);
int mpl_rng_get_seed(void);
#endif /* mpl_utils_h */
