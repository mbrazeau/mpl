//
//  mpl_taxoninfo.h
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_taxoninfo_h
#define mpl_taxoninfo_h

#include <string.h>

typedef struct _taxoninfo {
    long index;
    char* name;
} mpl_taxoninfo;

//mpl_taxoninfo* mpl_taxoninfo_new(void);
//int     mpl_taxoninfo_delete(mpl_taxoninfo** ti);
int     mpl_taxoninfo_set_name(mpl_taxoninfo* ti, const char* n);
int     mpl_taxoninfo_get_name(char* dest, mpl_taxoninfo* ti);
int     mpl_taxoninfo_set_index(mpl_taxoninfo* ti, long index);
long    mpl_taxoninfo_get_index(mpl_taxoninfo* ti);

#endif /* mpl_taxoninfo_h */
