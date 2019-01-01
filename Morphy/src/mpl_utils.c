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

void* safe_calloc(unsigned long nelems, size_t size)
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
