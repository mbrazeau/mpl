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
#include <stdbool.h>

#define MPLTAXNAMEMAX 100

typedef struct _taxoninfo {
    
    long  index;
    bool  isincluded;
    char* name;
    
} mpl_taxoninfo;

//mpl_taxoninfo* mpl_taxoninfo_new(void);
//int     mpl_taxoninfo_delete(mpl_taxoninfo** ti);
void    mpl_taxoninfo_cleanup(mpl_taxoninfo* ti);
int     mpl_taxoninfo_set_name(mpl_taxoninfo* ti, const char* n);
int     mpl_taxoninfo_get_name(char* dest, const mpl_taxoninfo* ti);
int     mpl_taxoninfo_set_index(mpl_taxoninfo* ti, const long index);
long    mpl_taxoninfo_get_index(const mpl_taxoninfo* ti);
int     mpl_taxoninfo_match(mpl_taxoninfo* ti1, mpl_taxoninfo* ti2);
int     mpl_taxoninfo_dummy(mpl_taxoninfo* ti);

#endif /* mpl_taxoninfo_h */
