//
//  mpl_bbreak.c
//  Morphy
//
//  Created by Martin Brazeau on 07/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#include "mpl_bbreak.h"
#include "mpl_scoretree.h"

sig_atomic_t search_interrupt = 0;

static int mpl_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base);
static void mpl_bbreak_trav_targets(mpl_node* n, const mpl_node* site, mpl_node** longlist, mpl_node** shortlist, long* i, long* j);
static long mpl_bbreak_get_target_list
(const mpl_tree* t, const mpl_node* site, mpl_bbreak* bbk);
static void mpl_swap_all(mpl_tree* t, mpl_bbreak* bbk);

void mpl_do_ratchet_search(mpl_tree* t, mpl_bbreak* bbk);

void do_interrupt(int signal)
{
    search_interrupt = 1;
}

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
    bbk->numreps = 1;
    bbk->num_rearrangs = 0;
    bbk->nratchets = 0;
    bbk->savelim = 0;
    bbk->bbktype = s->bbreak_type;
    bbk->treelist = s->treelist;
    bbk->numtaxa = s->num_taxa;
    
    // Set up all the node buffers:
    
    bbk->srcs = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    bbk->tgtslong = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    bbk->tgtsshort = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    bbk->clips = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    
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

void mpl_do_bbreak(mpl_bbreak* bbk)
{
    // Set up all the global variables.
    // Iterate over the tree list.
    long i = 0;
    long j = 0;
    long nstarts = 0;
    time_t timein;
    time_t timeout;
    mpl_topol* current = NULL;
    mpl_topol* top = NULL;
    mpl_tree* t = NULL;
    
    search_interrupt = 0;
    
    timein = (float)clock()/CLOCKS_PER_SEC;

    t = mpl_new_tree(bbk->numtaxa);
    
    mpl_stepwise_init(MPL_AST_RANDOM, bbk->numtaxa, 10, &bbk->stepwise);
    
//    bbk->numreps = 5;
    mpl_rng_set_seed(1); // TODO: Remove me!
    
    bbk->shortest = 0.0;
    
    for (i = 0; i < bbk->numreps; ++i) {
        
        printf("Replicate: %li\n", i+1);
        // If the buffer is empty get one or more trees by stepwise addition
        // Otheriwse, starttrees is the bbk buffer
//        mpl_rng_set_seed(1);
        printf("Random number seed: %u\n", mpl_rng_get_seed());
        mpl_stepwise_do_search(&bbk->stepwise);
        
        // Then get the trees from the stepwise struct
        
        nstarts = 1; // For now, only using one start tree, but a 'swap on all'
                     // could be programmed in the future.
        
        for (j = 0; j < nstarts; ++j) {

            // Add the first tree from the rep in the buffer
            top = mpl_treelist_get_shortest(bbk->stepwise.queued);
            mpl_tree_read_topol(t, top);
            mpl_tree_rebase(0, t);

//            // Initiate a new replicate
            current = mpl_treelist_newrep(true, t, bbk->treelist);

            // If the starting tree is already in the list, skip rep.
            if (current == NULL) {
                mpl_treelist_clear_rep(bbk->treelist);
                break;
            }
            
            if (bbk->nratchets > 0) {
                mpl_do_ratchet_search(t, bbk);
            } else {
                if (bbk->shortest == 0) {
                    bbk->shortest = current->score;
                }
    
                bbk->bestinrep = current->score;
                bbk->hitisland = false;
    
    //            mpl_swap_all(t, bbk);
                // For all unswapped trees in the buffer:
                do {
                    // Rebuild the tree according to the stored topology
                    printf("\r                                                                                ");
                    printf("\r\tShortest tree found: %.0f; swapping %li of %li trees saved.", bbk->bestinrep, current->index+1, bbk->treelist->num_trees);
                    fflush(stdout);
    
                    mpl_tree_read_topol(t, current);
                    //                t->score = mpl_fullpass_parsimony(t);
                    //                mpl_tree_rebase(0, t);
                    mpl_branch_swap(t, bbk);
    
                    current = mpl_treelist_get_next(bbk->treelist);
    
                    if (search_interrupt == 1) {
                        break;
                    }
    
                } while (current != NULL);
            }

            printf("\n\tReplicate completed.\n");
            printf("\tShortest tree found: %.0f steps\n", bbk->bestinrep);
            printf("\tNumber of trees saved: %li\n\n", bbk->treelist->rep_num_trees);

            // If no better equal or tree was found in this replicate,
            // wipe the rep
            if (bbk->bestinrep > bbk->shortest) {
                mpl_treelist_clear_rep(bbk->treelist);
            }

            if (search_interrupt == 1) {
                break;
            }
        }
        
        if (search_interrupt == 1) {
            break;
        }
    }
    
    timeout = (float)clock()/CLOCKS_PER_SEC;
    
    char* aborted_str = "aborted by the user after";
    char* completed_str = "completed in";
    char* compl_msg = completed_str;
    if (search_interrupt == 1) {
        compl_msg = aborted_str;
    }
    
    printf("\n\nHeuristic search %s %li seconds:\n", compl_msg, timeout - timein);
    printf("\t%lu rearrangements tried.\n", bbk->num_rearrangs);
    printf("\tShortest tree found: %.3f steps\n", bbk->shortest);
    printf("\t%li trees saved\n", bbk->treelist->num_trees);
    
    mpl_delete_tree(&t);

}

void mpl_do_ratchet_search(mpl_tree* t, mpl_bbreak* bbk)
{
    int i = 0;
    mpl_topol* current = NULL;
    double oldbest = 0.0;
    long oldsavelim = 0;
    
    printf("Initiating ratchet search\n");
    // Nixon 1:
    if (bbk->shortest == 0) {
        bbk->shortest = t->score;
    }
    
    bbk->savecount = 1;
    oldsavelim = bbk->savelim;
    bbk->savelim = 1;
    
    bbk->bestinrep = t->score;
    oldbest = bbk->bestinrep;
    bbk->hitisland = false;
    bbk->doislandcheck = true;
    
    // Swap the starting tree
    mpl_swap_all(t, bbk);
    
    // Nixon 2:
    bbk->nratchets = 50;
    
    mpl_treelist_reverse_head(bbk->treelist);
    current = mpl_treelist_get_next(bbk->treelist);
    
    mpl_tree_read_topol(t, current);
    
    mpl_treelist_clear_rep(bbk->treelist);
    
    for (i = 0; i < bbk->nratchets; ++i) {
        
        long index = 0;
        
        bbk->savecount = 0;
        
        printf("\tRatchet replication %i.\n", i + 1);
        
        mpl_treelist_newrep(false, t, bbk->treelist);
        
        // Nixon 3:
        mpl_do_ratchet_weights();

        // Nixon 4:
        // Swap the tree with new weights
        index = bbk->treelist->back->index; // for assert
        
        bbk->bestinrep = /*current->score =*/ mpl_length_only_parsimony(-1.0, t);
        bbk->doislandcheck = false;
        mpl_swap_all(t, bbk);
        bbk->doislandcheck = true;
        
        // Nixon 5;
        printf("Reset weights:\n");
        mpl_reset_std_weights();
        
        // Reset head to last tree in buffer
        bbk->savecount = 1;
        current = bbk->treelist->back;
        mpl_tree_read_topol(t, current);
        mpl_treelist_reverse_head(bbk->treelist);
        bbk->bestinrep = current->score = mpl_length_only_parsimony(-1.0, t);
        
        assert(current->index == index);
        
        mpl_swap_all(t, bbk);

        if (bbk->bestinrep < oldbest) {
            oldbest = bbk->bestinrep;
        }

        current = bbk->treelist->back;
        mpl_tree_read_topol(t, current);
        
        if (bbk->bestinrep > bbk->shortest) {
            mpl_treelist_clear_rep(bbk->treelist);
        }

        // Nixon 6
        printf("\nBest tree found: %.0f steps.\n", bbk->shortest);
    }
    
    bbk->savelim = oldsavelim;
}


void dbg_print_tree(mpl_node* n)
{
    if (n->tip > 0) {
        printf("%li", n->tip);
        return;
    }
    
    printf("(");
    dbg_print_tree(n->left);
    printf(",");
    dbg_print_tree(n->right);
    printf(")");
}


void mpl_branch_swap(mpl_tree* t, mpl_bbreak* bbk)
{
//    long ntax = 0; // The number of taxa in the tree
    long nnodes = 0; // The number of nodes in the tree
    long clipmax = 0;
    long tgtnum = 0;
    double score = 0.0;
//    double bound = 0.0;
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
    
   // bound = t->score;//bbk->shortest;
    
    mpl_tree_traverse(t); // Traverse the tree and get all nodes in the tree
    clips = bbk->clips;
    for (i = 1, clipmax = 0; i < t->size; ++i) {
        if (&t->nodes[i] != t->base && t->nodes[i].anc != t->base) {
            clips[clipmax] = &t->nodes[i];
            ++clipmax;
        }
    }
//    memcpy(clips, t->postord_all, t->size * sizeof(mpl_node*));
    nnodes = t->size;
//    clipmax = nnodes-1; /*NOTE: leaving out base!!!*/
//    clipmax = j;
    // TODO: clipmax-1 is temporary (root-adjacent clips need to be fixed)
    for (i = 0; i < clipmax; ++i) { // NOTE: Assumes 'unrooted' tree!!!
        
        signal(SIGINT, do_interrupt);
        
        if (search_interrupt == 1) {
            return;
        }
        
        if (clips[i]->lock == true) {
            clips[i]->lock = false;
            continue;
        }
        
        clips[i]->clipmark = true;
        
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
        
        double tgtlen = 0.0;
        double srclen = 0.0;

        tgtlen = mpl_fullpass_parsimony(t);
        
        if ((*src)->tip == 0) {
            srclen = mpl_fullpass_subtree(*src, t);
        }
//        if ((srclen + tgtlen) >= bbk->shortest) {
//            mpl_node_bin_connect(left, right, clips[i]);
//            clips[i]->lock = false;
//            clips[i]->clipmark = false;
//            continue;
//        }
        // If the branch is zero-length, no need to continue. All swaps will
        // result in redundant trees after collapsing.
//        if ((srclen + tgtlen) == bound) {
//            mpl_node_bin_connect(left, right, clips[i]);
//            clips[i]->lock = false;
//            clips[i]->clipmark = false;
//            continue;
//        }
        
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
        
        // Make a list of target sites
        mpl_bbreak_get_target_list(t, csite, bbk);
        
        src = bbk->srcs;
        
        mpl_node* curnbr = NULL;
        mpl_node* oldnbr = NULL;
        
        for (src = bbk->srcs; src < srcs; ++src) {
        
            curnbr = *src;
            
            // Re-root the tree
            mpl_bbreak_tbr_reroot(*src, clips[i]);
            
            if (clips[i]->tip == 0) {
                if (oldnbr != NULL) {
                    if (oldnbr->tip > 0) {
                        oldnbr = oldnbr->anc;
                    }
                    mpl_update_src_actives(oldnbr, clips[i]);
                }
            }
            mpl_src_root_parsimony(clips[i]);
            
            oldnbr = curnbr;
            
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
                
                if (search_interrupt == 1) {
                    return;
                }
                
                mpl_node_bin_connect(tgts[j], NULL, clips[i]);
                ++bbk->num_rearrangs;
                
                score = mpl_score_try_parsimony(tgtlen + srclen, bbk->bestinrep, clips[i], tgts[j], t);
                t->score = (score + tgtlen + srclen);

//                t->score = mpl_fullpass_parsimony(t);
//                t->score = mpl_length_only_parsimony(bbk->shortest, t);
                
                if (bbk->bestinrep == 0.0) {
                    bbk->bestinrep = t->score;
                }
                
                if (t->score <= bbk->bestinrep) {
                    
                    if (t->score < bbk->bestinrep) {
                        
                        if (t->score < bbk->shortest) {
                            
                            bbk->shortest = t->score;
                            bbk->bestinrep = t->score;
                            mpl_treelist_clear_all(bbk->treelist);
                            clips[i]->clipmark = false;
                            mpl_treelist_add_tree(false, t, bbk->treelist);
                            clips[i]->lock = false;
                        
                        }
                        else {
                        
                            mpl_topol* ret = 0;
                            bbk->bestinrep = t->score;
                            mpl_treelist_clear_rep(bbk->treelist);
                            clips[i]->clipmark = false;
                            ret = mpl_treelist_add_tree(bbk->doislandcheck, t, bbk->treelist);
                            clips[i]->lock = false;
                            
                            if (ret != NULL && bbk->doislandcheck == true) {
                                if (t->score == bbk->shortest) {
                                    bbk->hitisland = true;
                                    return;
                                }
                            }
                        }
                        
                        bbk->savecount = 1;
                        
                        return;
                    }
                    else if (bbk->savelim > 0) {
                        if (bbk->treelist->rep_num_trees < bbk->savelim)  {
                            mpl_treelist_add_tree(true, t, bbk->treelist);
                        }
                    }
                    else {
                        mpl_treelist_add_tree(true, t, bbk->treelist);
                    }
                    
                }
            
                //  Put the src tree back in its original spot
                mpl_node_bin_clip(clips[i]);
            }
        }
        
        if (search_interrupt == 1) {
            return;
        }
        
        // Reset to the old spot
        if (clips[i]->tip == 0) {
            mpl_bbreak_tbr_reroot(rtlef, clips[i]);
            if (clips[i]->left != rtlef) {
                mpl_node_rotate(clips[i]);
            }
            if (clips[i]->left != rtlef || clips[i]->right != rtrig) {
                mpl_bbreak_tbr_reroot(rtrig, clips[i]);
            }
        }
       
        // Restore the tree to exactly as it was before
        mpl_node_bin_connect(left, right, clips[i]);
        clips[i]->lock = false;
        clips[i]->clipmark = false;
    }
}

/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/

static void mpl_swap_all(mpl_tree* t, mpl_bbreak* bbk)
{
    // For all unswapped trees in the buffer:
    mpl_topol* current = NULL;
    current = mpl_treelist_get_next(bbk->treelist);
    
    do {
        // Rebuild the tree according to the stored topology
        printf("\r                                                                                ");
        printf("\r\tShortest tree found: %.0f; swapping %li of %li trees saved.",
               bbk->bestinrep, current->index+1, bbk->treelist->num_trees);
        fflush(stdout);
        
        mpl_tree_read_topol(t, current);
        //                t->score = mpl_fullpass_parsimony(t);
        //                mpl_tree_rebase(0, t);
        mpl_branch_swap(t, bbk);
        
        current = mpl_treelist_get_next(bbk->treelist);
        
        if (search_interrupt == 1) {
            break;
        }
        
    } while (current != NULL);
}

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
    
//    if (n->lock == true) {
//        return;
//    }
    
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

//#ifdef DEBUG
int mpl_test_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base)
{
    return mpl_bbreak_tbr_reroot(tgtnbr, base);
}

void mpl_test_branch_swap(mpl_tree* t, mpl_bbreak* bbk)
{
    mpl_branch_swap(t, bbk);
}
//#endif
