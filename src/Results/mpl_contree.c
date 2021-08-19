//
//  mpl_contree.c
//  morphy_nui_v03
//
//  Created by Martin Brazeau on 19/08/2021.
//
#include <assert.h>

#include "mpl_contree.h"
#include "mpl_tree.h"
#include "mpl_bipartlist.h"
#include "mpl_treelist.h"

mpl_contree *mpl_contree_new(int ntax, mpl_treelist *tl)
{
    mpl_contree *newct = NULL;
    
    newct->t = mpl_new_tree(ntax);
    
    newct->biparts = mpl_bipartlist_new(ntax);
    
    newct->treelist = tl;
    
    return newct;
}

void mpl_contree_delete(mpl_contree *ctree)
{
    mpl_delete_tree(&ctree->t);
    
    // TODO: Other cleanup
}

void mpl_contree_strict(mpl_contree* ctree)
{
    assert(ctree != NULL);
    
    int i = 0;
    mpl_node *n;
    mpl_node **p;
    
    mpl_topol *top = mpl_treelist_get_topol(0, ctree->treelist);
    
    mpl_tree_read_topol(ctree->t, top);
    
    mpl_tree_traverse(ctree->t);
    
    for (i = 0; i < ctree->t->num_taxa; ++i) {
        n = &ctree->t->nodes[i];
        mpl_bitset_set(n->mem_index, ctree->biparts->biparts[i]);
    }
    
    for (i = 0; i < ctree->t->nintern; ++i) {
        n = ctree->t->postord_intern[i];
        p = n->descs;
        do {
            mpl_bitset_OR(ctree->biparts->biparts[n->mem_index], ctree->biparts->biparts[n->mem_index], ctree->biparts->biparts[(*p)->mem_index]);
            ++p;
        } while (*p);
    }
    
    for (i = 1; i < mpl_treelist_get_numtrees(ctree->treelist); ++i) {
        //Rebuild the tree from memory
        top = mpl_treelist_get_topol(i, ctree->treelist);
        mpl_tree_read_topol(ctree->t, top);
        mpl_tree_traverse(ctree->t);
        mpl_tree_calc_biparts(ctree->t);
        
        // Now check that all internal biparts in the tree can be found in the bipartition list
    }
}
