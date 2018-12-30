//
//  mpl_taxoninfo.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <stdlib.h>
#include "mpl_taxoninfo.h"
#include "../mpl_utils.h"

int mpl_taxoninfo_set_name(mpl_taxoninfo* ti, const char* n)
{
    if (!ti) {
        return -1;
    }
    
    // Delete previous name as it will be overwritten
    mplfree(ti->name);
    
    size_t len = 0;
    
    len = strlen(n) + 1;
    
    ti->name = (char*)safe_calloc(len, sizeof(char));
    
    strcpy(ti->name, n);
    
    return 0;
}

int mpl_taxoninfo_get_name(char* dest, mpl_taxoninfo* ti)
{
    int ret = 0;
    
    if (!ti) {
        return -1;
    }
    
    if (!dest) {
        return -2;
    }
    
    if (ti->name) {
        strcpy(dest, ti->name);
        ret = 0;
    }
    else {
        ret = -3;
    }
    
    return ret;
}

int mpl_taxoninfo_set_index(mpl_taxoninfo* ti, long index)
{
    if (!ti) {
        return -1;
    }
    
    ti->index = index;
    
    return 0;
}

long mpl_taxoninfo_get_index(mpl_taxoninfo* ti)
{
    if (ti) {
        return ti->index;
    }
    
    return -1;
}
