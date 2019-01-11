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
(const mpl_tree* t, const mpl_node* site, mpl_node** longlist, mpl_node** shortlist);


void mpl_do_bbreak(mpl_tree* t, mpl_bbreak* bbk)
{
    // Set up all the global variables.
    // Iterate over the tree list.
}

void mpl_branch_swap(mpl_tree* t, mpl_bbreak* bbk)
{
    long ntax = 0; // The number of taxa in the tree
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
        bbk->nlongtgts = mpl_bbreak_get_target_list(t, csite, bbk->tgtslong, bbk->tgtsshort);
        // Short list is counted based on whether the subtree clipped was a tip.
        if (clips[i]->tip) {
            bbk->nshorttgts = bbk->nlongtgts-4;
        }
        else {
            bbk->nshorttgts = bbk->nlongtgts-2;
        }
        assert(bbk->nshorttgts >= 0);
        
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
(const mpl_tree* t, const mpl_node* site, mpl_node** longlist, mpl_node** shortlist)
{
    assert(t->num_polys == 0);
    
    mpl_node* start = mpl_node_get_sib(&t->nodes[0]);
    
    long i = 0;
    long j = 0;
    mpl_bbreak_trav_targets(start, site, longlist, shortlist, &i, &j);
    
    return i; // The shortlist length is i - 4 if clip site is internal and
    // i - 2 if it's a tip.
}
