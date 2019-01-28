//
//  mpl_bbreak.c
//  Morphy
//
//  Created by Martin Brazeau on 07/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <assert.h>
#include <string.h>

#include <stdio.h>

#include "mpl_bbreak.h"

static int mpl_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base);
static void mpl_bbreak_trav_targets(mpl_node* n, const mpl_node* site, mpl_node** longlist, mpl_node** shortlist, long* i, long* j);
static long mpl_bbreak_get_target_list
(const mpl_tree* t, const mpl_node* site, mpl_bbreak* bbk);

/*
 *  PUBLIC FUNCTION DEFINITIONS
 */

void mpl_temp_traver_subtr(mpl_node* n)
{
    if (n->tip) {
        printf("%li", n->tip);
        return;
    }
    printf("(");
    mpl_temp_traver_subtr(n->left);
    printf(",");
    mpl_temp_traver_subtr(n->right);
    printf(")");
}


int mpl_bbreak_init(mpl_search* s, mpl_bbreak* bbk)
{
    int ret = 0;
    
    // TODO: Set size based on inputs from s
    bbk->num_rearrangs = 0;
    bbk->bbktype = s->bbreak_type;
    bbk->treelist = s->treelist;
    
    // Set up all the node buffers:
    
    bbk->srcs = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    bbk->tgtslong = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    bbk->tgtsshort = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    
    return ret;
}

void mpl_bbreak_reset(mpl_bbreak* bbk)
{
    bbk->bbktype = DEFAUL_BBREAK;
    bbk->num_nodes = 0;
    safe_free(bbk->srcs);
    bbk->nlongtgts = 0;
    safe_free(bbk->tgtslong);
    bbk->nshorttgts = 0;
    safe_free(bbk->tgtsshort);
    bbk->num_rearrangs = 0;
    bbk->treelist = NULL;
}

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
    long tgtnum = 0;
    
    mpl_node* left;
    mpl_node* right;
    mpl_node* csite;
    mpl_node* rtlef = NULL;
    mpl_node* rtrig = NULL;
    mpl_node** clips = NULL;
    mpl_node** srcs = bbk->srcs;
    mpl_node** src = srcs;
    mpl_node** tgts;
    long i = 0;
    long j = 0;
    
    mpl_tree_traverse(t); // Traverse the tree and get all nodes in the tree
    
    clips = t->postord_all; // The postorder list of nodes in the tree
    nnodes = t->size;
    clipmax = nnodes-1; /*NOTE: leaving out base!!!*/
    
    for (i = 1; i < clipmax; ++i) { // NOTE: Assumes 'unrooted' tree!!!
        
        // Clip the tree at i
        left = NULL;
        right = NULL;
        csite = NULL;
        
        csite = mpl_node_get_sib(clips[i]);
        
        if (clips[i]->anc->left == clips[i]) {
            right = csite;
            left = NULL; // Simply precautionary
        }
        else {
            left = csite;
            right = NULL;
        }
        
        mpl_node_bin_clip(clips[i]);
        *src = clips[i];
        srcs = bbk->srcs;
        
        // << Reoptimise the subtrees as quickly as possible >>
        
        // <<< Check the cost >>> of reinserting the node at this clip
        // If the result is no difference on the length of the
        // tree, this branch is zero-length and can be skipped
        
        // Set up the src pointers
        if ((*src)->tip == 0) {
            rtlef = clips[i]->left;
            rtrig = clips[i]->right;
            *srcs = clips[i]->left;
            ++srcs;
            if (clips[i]->right->tip == 0) {
                *srcs = clips[i]->right->left;
                ++srcs;
                *srcs = clips[i]->right->right;
                ++srcs;
            }
        }
        else {
            *srcs = clips[i];
            ++srcs;
        }
        
        // Size of the long list is determined by the traversal
        mpl_bbreak_get_target_list(t, csite, bbk);
        
        for (src = bbk->srcs; src < srcs; ++src) {
            
            // Re-root the tree
            mpl_bbreak_tbr_reroot(*src, clips[i]);
            
            // Get any additional nodes needed for unique rerootings
            if ((*src)->tip == 0) {
                *srcs = (*src)->left;
                ++srcs;
                *srcs = (*src)->right;
                ++srcs;
            }
            
            // The first time round, the subtree won't get rerooted, so only
            // move around to sites that don't neighbor the original clipsite
            if (src == bbk->srcs) {
                tgtnum = bbk->nshorttgts;
                tgts = bbk->tgtsshort;
            }
            else {// Try all target sites
                // For SPR: If this is the first re-rooting, move to all sites
                // on the short list. Otherwise, don't move anywhere
                if (bbk->bbktype == MPL_SPR_T) {
                    tgtnum = 1;
                    tgts = &csite;
                }
                else {
                    tgtnum = bbk->nlongtgts;
                    tgts = bbk->tgtslong;
                }
            }
            
            for (j = 0; j < tgtnum; ++j ){
                
                mpl_node_bin_connect(tgts[j], NULL, clips[i]);
                
                ++bbk->num_rearrangs;
                // for each target site <<< check the length >>> of the tree {
                
                //  Sort storage or discarding of try based on length
                //                mpl_treelist_add_tree(t, bbk->treelist);
                
                mpl_treelist_add_tree(false, t, bbk->treelist);
                
                //  Put the src tree back in its original spot
                mpl_node_bin_clip(clips[i]);
                
            }
        }
        
        // Reset to the old spot
        if (clips[i]->tip == 0) {
            mpl_bbreak_tbr_reroot(rtlef, clips[i]);
            if (clips[i]->left != rtlef) {
                mpl_node_rotate(clips[i]);
//                mpl_node* t = NULL;
//                t = clips[i]->left;
//                clips[i]->left = clips[i]->right;
//                clips[i]->right = t;
            }
            mpl_bbreak_tbr_reroot(rtrig, clips[i]);
        }
       
        // Restore the tree to exactly as it was before
        mpl_node_bin_connect(left, right, clips[i]);
    }
}

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

static int mpl_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base)
{
    if (tgtnbr->anc == base || base->tip > 0) {
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
    if (start == NULL) {
        bbk->nshorttgts = 0;
        bbk->nlongtgts  = 1;
        bbk->tgtslong[0] = &t->nodes[0];
        return 1;
    }
    
    bbk->nshorttgts = 0;
    bbk->nlongtgts  = 0;
    mpl_bbreak_trav_targets
    (start, site, bbk->tgtslong, bbk->tgtsshort, &bbk->nlongtgts, &bbk->nshorttgts);
    
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

void mpl_test_branch_swap(mpl_tree* t, mpl_bbreak* bbk)
{
    mpl_branch_swap(t, bbk);
}
#endif
