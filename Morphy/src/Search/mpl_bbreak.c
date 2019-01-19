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
static void mpl_bbreak_trav_targets(mpl_node* n, const mpl_node* site, mpl_node** longlist, mpl_node** shortlist, long* i, long* j);
static long mpl_bbreak_get_target_list
(const mpl_tree* t, const mpl_node* site, mpl_bbreak* bbk);

/*
 *  PUBLIC FUNCTION DEFINITIONS
 */

void mpl_do_bbreak(mpl_tree* t, mpl_bbreak* bbk)
{
    // Set up all the global variables.
    // Iterate over the tree list.
}

void mpl_branch_swap(mpl_tree* t, mpl_bbreak* bbk)
{
//    long ntax = 0; // The number of taxa in the tree
    long nnodes = 0; // The number of nodes in the tree
    long clipmax = 0;
    
    mpl_node** clips = NULL;
    mpl_node* srcs = *bbk->srcs;
    mpl_node* src = srcs;
    long i = 0;
    long j = 0;
    
    mpl_tree_traverse(t); // Traverse the tree and get all nodes in the tree
    clips = t->postord_all; // The postorder list of nodes in the tree
    nnodes = t->tree_size;
    clipmax = nnodes-1; /*NOTE: leaving out base!!!*/
    
    for (i = 1; i < clipmax; ++i) { // NOTE: Assumes 'unrooted' tree!!!
        
        // Clip the tree at i
        mpl_node* left = NULL;
        mpl_node* right = NULL;
        mpl_node* csite = NULL;
        
        csite = mpl_node_get_sib(clips[i]);
        
        if (clips[i]->anc->left == clips[i]) {
            right = csite;
        }
        else {
            left = csite;
        }
        mpl_node_bin_clip(clips[i]);
        
        // <<< Check the cost >>> of reinserting the node at this clip
        // If the result is no difference on the length of the
        // tree, this branch is zero-length and can be skipped
        
        // Get all the target sites:
        long tgtnum = 0;
        
        // Size of the long list is determined by the traversal
        mpl_bbreak_get_target_list(t, csite, bbk);
        
        // Now, set tgtnumb based on whether we are doing SPR or TBR...
        
        for (j = 0; j < tgtnum; ++j ){
            
            // For each rerooting site
            while (src != srcs) {
                
                // Re-root the tree
                
                // Try all target sites
                // for each target site <<< check the length >>> of the tree {
                
                //  Sort storage or discarding of try based on length
                mpl_treelist_add_tree(t, bbk->treelist);
                
                //  Put the src tree back in its original spot
                
                ++src;
            }
            
        }
       
        // Restore the tree to exactly as it was before
        if (left != NULL) {
            mpl_node_bin_connect(left, NULL, clips[i]);
        }
        else {
            mpl_node_bin_connect(NULL, right, clips[i]);
        }
    }
}

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

static int mpl_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base)
{
    if (tgtnbr->anc == base) {
        return 1;
    }
    
    mpl_node*   n   = NULL;
    mpl_node*   p   = NULL;
    mpl_node*   q   = NULL;
    mpl_node*   r   = NULL;
    mpl_node**  rp  = NULL;
    
    mpl_node* n1 = tgtnbr;
    mpl_node* n2 = tgtnbr->anc;
    mpl_node* banc = base->anc;
    
    n = n1->anc;
    p = n1;
    
    base->anc = NULL;
    
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
    
//    r   = NULL;
    rp  = NULL;
    q = base->left;
    
    if (q == NULL) {
        q = base->right;
    }
    
#ifdef DEBUG
    assert(q);
#endif
    
    q->anc = base->anc;
    
    // Find the pointer to the base:
    if (q->anc->right == base) {
        rp = &q->anc->right;
    }
    else {
        rp = &q->anc->left;
#ifdef DEBUG
//        assert((*rp)->left == base);
#endif
    }
    
    *rp = q;
    
    base->left = n1;
    n1->anc = base;
    
    base->right = n2;
    n2->anc = base;
    
    base->anc = banc;
    return 0;
}

static void mpl_bbreak_trav_targets
(mpl_node* n,
 const mpl_node* site,
 mpl_node** longlist,
 mpl_node** shortlist,
 long* i,
 long* j)
{
    longlist[*i] = n;
    ++(*i);
    
    if (n != site && n->anc != site && site->anc != n && site->anc != n->anc) {
        shortlist[*j] = n;
        ++(*j);
    }
    
    if (!n->tip) {
        mpl_bbreak_trav_targets(n->left, site, longlist, shortlist, i, j);
        mpl_bbreak_trav_targets(n->right, site, longlist, shortlist, i, j);
    }
}


static long mpl_bbreak_get_target_list
(const mpl_tree* t, const mpl_node* site, mpl_bbreak* bbk)
{
    assert(t->num_polys == 0);
    
    mpl_node* start = mpl_node_get_sib(&t->nodes[0]);
    
    bbk->nshorttgts = 0;
    bbk->nlongtgts = 0;
    mpl_bbreak_trav_targets(start, site, bbk->tgtslong, bbk->tgtsshort, &bbk->nlongtgts, &bbk->nshorttgts);
    
    return 0; 
}

/*
 *  TEST INTERFACE FUNCTION DEFINITIONS
 */

#ifdef DEBUG
int mpl_test_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base)
{
    return mpl_bbreak_tbr_reroot(tgtnbr, base);
}
#endif
