//
//  mpl_contree.c
//  morphy_nui_v03
//
//  Created by Martin Brazeau on 19/08/2021.
//
#include <assert.h>
#include <string.h>

#include "mpl_tree.h"
#include "mpl_bipartlist.h"
#include "mpl_treelist.h"
#include "mpl_contree.h"

static void mpl_contree_collapse_nodes(int numtrees, mpl_contree *ctree);

mpl_contree *mpl_contree_new(int ntax, mpl_treelist *tl)
{
    mpl_contree *newct = NULL;
    
    newct = safe_calloc(1, sizeof(mpl_contree));
    
    // TODO: Check calloc success etc.
    
    newct->t = mpl_new_tree(ntax);
    newct->t->num_polys = 1;
    
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
    int j = 0;
    int k = 0;
    mpl_node *n;
    mpl_node **p;
    mpl_tree *t = mpl_new_tree(ctree->t->num_taxa);
    // TODO: Error if failed.
    
    ctree->biparts->num_splits = 0;
    
    memset(ctree->biparts->counts, 0, ctree->biparts->max_splits * sizeof(int));
    mpl_topol *top = mpl_treelist_get_topol(0, ctree->treelist);
    
    mpl_tree_read_topol(ctree->t, top);
    
    mpl_tree_traverse(ctree->t);
    
    // Input the tip "bipartitions" (could be done before entering the function)
    for (i = 0; i < ctree->t->num_taxa; ++i) {
        n = &ctree->t->nodes[i];
        mpl_bitset_set(n->mem_index, ctree->biparts->biparts[i]);
        ctree->biparts->num_splits++;
    }
    
    // For the first tree, calculate all the core bipartitions
    for (i = 0; i < ctree->t->nintern; ++i) {
        n = ctree->t->postord_intern[i];
        p = n->descs;
        do {
            mpl_bitset_OR(ctree->biparts->biparts[n->mem_index], ctree->biparts->biparts[n->mem_index], ctree->biparts->biparts[(*p)->mem_index]);
            ++p;
        } while (*p);
        ctree->biparts->indexes[i] = n->mem_index;
        ctree->biparts->num_splits++;
    }
   
    // Loop over the remaining trees in the tree list (hence starting at 1)
    for (i = 1; i < mpl_treelist_get_numtrees(ctree->treelist); ++i) {
        //Rebuild the tree from memory
        top = mpl_treelist_get_topol(i, ctree->treelist);
        mpl_tree_read_topol(t, top);
        mpl_tree_traverse(t);
        mpl_tree_calc_biparts(t);
        
        // Check that all internal biparts in the tree can be found in the bipartition list
        for (j = 0; j < t->nintern; ++j) {
            n = t->postord_intern[j];
            for (k = 0; k < ctree->biparts->num_splits; ++k) {
                if (!mpl_bitset_cmp(n->bipart, ctree->biparts->biparts[k])) {
                    ctree->biparts->counts[k] += 1;
                }
            }
        }
    }
    
    // Now, collapse any branches not represented in 100% of the input trees
    mpl_contree_collapse_nodes(mpl_treelist_get_numtrees(ctree->treelist), ctree);
    
    mpl_delete_tree(&t);
}


static void mpl_contree_collapse_nodes(int numtrees, mpl_contree *ctree)
{
    int i = 0;
    mpl_node *n = NULL;
    
    for (i = 0; i < ctree->t->nintern-1; ++i) {
        n = ctree->t->postord_intern[i];
        if (ctree->biparts->counts[n->mem_index] < numtrees-1) {
            mpl_node_collapse(n);
        }
    }
}
