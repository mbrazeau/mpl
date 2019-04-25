//
//  mpl_treelist.h
//  Morphy
//
//  Created by Martin Brazeau on 09/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_treelist_h
#define mpl_treelist_h

#include <stdbool.h>

#include "mpl_topol.h"
#include "mpl_tree.h"

typedef struct _treelist {
    
    long        num_taxa;
    long        num_trees;
    long        max_trees;
    long        increase_rate;
    double      shortest;
    double      longest;
    long        rep_num_trees;
    long        rep_index;
    mpl_topol*  trees;
    mpl_topol*  front;
    mpl_topol*  back;
    mpl_topol*  head;
    mpl_topol*  repstart;
    
} mpl_treelist;

mpl_treelist*   mpl_treelist_new(const long num_taxa, const long max_trees, const long increase_rate);
void            mpl_treelist_delete(mpl_treelist** tl);
mpl_topol*      mpl_treelist_add_tree(const bool checkdupes, mpl_tree* t, mpl_treelist* tl);
long            mpl_treelist_get_numtrees(const mpl_treelist* tl);
void            mpl_treelist_overwrite_longest(mpl_tree* t, mpl_treelist* tl);
mpl_topol*      mpl_treelist_get_topol(long tnum, mpl_treelist* tl);
void            mpl_treelist_reset_head(mpl_treelist* tl);
void mpl_treelist_restart_rep(mpl_treelist* tl);
void            mpl_treelist_reverse_head(mpl_treelist* tl);
mpl_topol*      mpl_treelist_get_next(mpl_treelist* tl);
mpl_topol*      mpl_treelist_new_rep(mpl_treelist* tl);
mpl_topol*      mpl_treelist_new_subrep(mpl_treelist* tl);
mpl_topol*      mpl_treelist_get_shortest(mpl_treelist* tl);
//void            mpl_treelist_clear_rep(mpl_treelist* tl);
void            mpl_treelist_clear_all(mpl_treelist* tl);
mpl_topol*      mpl_treelist_newrep(bool checknew, mpl_tree* t, mpl_treelist* tl);
void            mpl_treelist_clear_rep(mpl_treelist* tl);

//void            mpl_treelist_remove_topol(mpl_topol* top, mpl_treelist* tl);


#endif /* mpl_treelist_h */
