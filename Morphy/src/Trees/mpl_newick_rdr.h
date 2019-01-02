//
//  mpl_newick_rdr.h
//  Morphy
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_newick_rdr_h
#define mpl_newick_rdr_h

#include "mpl_topol.h"

typedef struct _nwk_rdr {
    long nopenbr;
    long nclosebr;
    long ncomma;
    long nspace;
    long taxnamemax; // Maximum length of a taxon name
    char* namebuffer;
    long** descindices; // At each rank of Newick hierarchy, store the descendant returns
} mpl_newick_rdr;

int mpl_newick_rdr_init(long num_taxa, mpl_newick_rdr* rdr);
int mpl_newick_read(const char* nwkstr, mpl_topol* top, mpl_newick_rdr* rdr);
#endif /* mpl_newick_rdr_h */
