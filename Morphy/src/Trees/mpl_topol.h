//
//  mpl_topol.h
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_topol_h
#define mpl_topol_h

typedef struct mpl_topol mpl_topol;
typedef struct mpl_topol {
    long        num_taxa;
    long        num_nodes;
    long        num_polys;
    long        root; // If rooted, then give the index, otherwise -1
    
    /* Store the tree as an index-ordered "edge table", where a 0 or
     * positive integer is the memory index of the ancestor. -1 is
     * used by convention to indicate no ancestor in the tree (i.e.
     * refers to a node not included in the tree. */
    long*       edges;
    mpl_topol*  next;
    mpl_topol*  back;
} mpl_topol;

mpl_topol*  mpl_topol_new(long num_taxa);
int         mpl_topol_delete(mpl_topol** top);
int         mpl_topol_reset(long num_taxa, mpl_topol* top);
int         mpl_topol_link(mpl_topol* parent, mpl_topol* child);
int         mpl_topol_compare(mpl_topol* t1, mpl_topol* t2);


#endif /* mpl_topol_h */