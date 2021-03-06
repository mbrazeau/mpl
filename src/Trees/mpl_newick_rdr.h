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
    
} mpl_newick_rdr;

mpl_newick_rdr* mpl_newick_rdr_new(const long num_taxa);
void mpl_newick_rdr_delete(mpl_newick_rdr** rdr);
int mpl_newick_rdr_init(const long num_taxa, mpl_newick_rdr* rdr);
int mpl_newick_rdr_cleanup(mpl_newick_rdr* rdr);
int mpl_newick_read(const char* nwkstr, mpl_topol* top, mpl_newick_rdr* rdr);
#endif /* mpl_newick_rdr_h */
