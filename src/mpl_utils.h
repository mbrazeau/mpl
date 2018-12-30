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

#define mplfree(x) {if(x) {free(x); x  =  NULL;}}

void* safe_calloc(unsigned long nelems, size_t size);

#endif /* mpl_utils_h */
