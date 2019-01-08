//
//  mpl_bbreak.c
//  Morphy
//
//  Created by Martin Brazeau on 07/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <assert.h>

#include "mpl_bbreak.h"

static int mpl_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base);

void mpl_do_bbreak(mpl_tree* t, mpl_bbreak* bbk)
{
    // Set up all the global variables.
    // Iterate over the tree list.
}

//void mpl_branch_swap(mpl_tree* t, mpl_bbreak* bbk)
//{
//    long ntax = 0; // The number of taxa in the tree
//    long nnodes = 0; // The number of nodes in the tree
//    mpl_node** nodes = t->postord_all; // The postorder list of nodes in the tree
//    mpl_node* srcs = *bbk->srcs;
//    mpl_node* src = srcs;
//    mpl_node* tgts = *bbk->tgts;
//    mpl_node* tgt = tgts;
//    
//    long i = 0;
//    
//    for (i = 1; i < nnodes; ++i) { // Assumes 'unrooted' tree
//        // Clip the tree at i
//        
//        // <<< Check the cost >>> of reinserting the node at this clip
//        // If the result is no difference on the length of the
//        // tree, this branch is zero-length and can be skipped
//        
//        // For each rerooting site
//        while (src != srcs) {
//            
//            // Re-root the tree
//            
//            // Try all target sites
//            // for each target site <<< check the length >>> of the tree {
//            //  Sort storage or discarding of try based on length
//            //  Put the src tree back in its original spot
//            
//            ++src;
//        }
//        
//        // Restore the tree to exactly as it was before
//    }
//}

static int mpl_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base)
{
    if (tgtnbr == base) {
        return 1;
    }
    
    mpl_node *n     = NULL;
    mpl_node *p     = NULL;
    mpl_node *q     = NULL;
    mpl_node *r     = NULL;
    mpl_node** rp   = NULL;
    
    mpl_node* n1 = tgtnbr;
    mpl_node* n2 = tgtnbr->anc;
    
    n = n1->anc;
    p = n1;
    
    while (n) {
        // TODO: This can be simplified without pointers to pointers by using
        // an intermediate variable. But for now this should work.
        q = n->anc;
        if (n->left == p) {
            r = n->left;
            rp = &n->left;
        }
        else {
            r = n->right;
            rp = &n->right;
        }
        *rp = n->anc;
        n->anc = r;
        p = n;
        n = q;
    }
    
    r   = NULL;
    rp  = NULL;
    
    if (base->left == NULL) {
        r = n->right;
    }
    else {
        r = n->left;
    }
#ifdef DEBUG
    assert(r);
#endif
    
    if (base->anc->right == base) {
        rp = &base->anc->right;
    }
    else {
        rp = &base->anc->right;
#ifdef DEBUG
        assert((*rp)->left == base);
#endif
    }
    
    r->anc = base->anc;
    *rp = r;
    
    base->left = n1;
    n1->anc = base;
    
    base->right = n2;
    n2->anc = base;
    
    return 0;
}
