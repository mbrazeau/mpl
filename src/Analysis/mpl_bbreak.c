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
#include <signal.h>

#include "mpl_bbreak.h"
#include "mpl_scoretree.h"

int search_interrupt = 0;

static void mpl_bbreak_print_status_header(mpl_bbreak* bbk);
static void mpl_bbreak_print_status(long rep, mpl_bbreak* bbk);
static int mpl_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base);
static void mpl_bbreak_trav_targets
(mpl_node* n, const mpl_node* site, mpl_node** longlist, mpl_node** shortlist, long* i, long* j);
static long mpl_bbreak_get_target_list(const mpl_tree* t, const mpl_node* site, mpl_bbreak* bbk);
static void mpl_swap_all(const bool report, mpl_tree* t, mpl_bbreak* bbk);
void mpl_do_ratchet_search(mpl_tree* t, mpl_bbreak* bbk);
void mpl_bbreak_incr_rearrangs(mpl_bbreak* bbk);
void mpl_bbreak_reset_rearrangs(mpl_bbreak* bbk);
double mpl_bbreak_get_rearrangs(mpl_bbreak* bbk);

void do_interrupt(int signal)
{
    search_interrupt = 1;
}

/*
 *  PUBLIC FUNCTION DEFINITIONS
 */

mpl_bbreak* mpl_bbreak_new(long num_nodes)
{
    mpl_bbreak* nbbk = NULL;
    
    nbbk = (mpl_bbreak*)safe_calloc(1, sizeof(mpl_bbreak));

    return nbbk;
}

int mpl_bbreak_search(mpl_search* s)
{
    assert(s != NULL);
    mpl_bbreak_init(s, s->bbreak);
    
    // Check that search can happen
    if (mpl_treelist_get_numtrees(s->treelist) < 1) {
        if (s->stepw_type == MPL_AST_INMEM) {
            return MPL_NOTREES;
        }
    }
    
    mpl_do_bbreak(s->bbreak);
    mpl_bbreak_cleanup(s->bbreak);
    
    return MPL_SUCCESS;
}

int mpl_bbreak_init(mpl_search* s, mpl_bbreak* bbk)
{
    int ret = 0;
    
    // TODO: Set size based on inputs from s
    bbk->num_rearrangs  = 0;
    bbk->nratchets      = 0;
    bbk->savelim        = 0;
    bbk->nsubs          = 0;
    bbk->maxsubs        = 0;
    bbk->maxhits        = 5;
    bbk->nhits          = 0;
    bbk->bbktype        = s->bbreak_type;
    bbk->treelist       = s->treelist;
    bbk->numtaxa        = s->num_taxa;
    bbk->num_nodes      = 2 * s->num_taxa - 1;
    bbk->doislandcheck  = true;
    bbk->limit          = 0.0;
    bbk->bestinrep      = MPL_MAXSCORE;
    bbk->shortest       = MPL_MAXSCORE;
    
    // Check if bbreak is a replication type and set numreps
    if (mpl_search_is_reptype(s) == true) {
        bbk->numreps = s->num_reps;
    }
    else {
        bbk->numreps = 1;
    }
    
    if (s->use_ratchet == true) {
        bbk->nratchets = s->ratch_iter;
        mpl_stepwise_init(MPL_AST_RANDOM, bbk->numtaxa, 1, &bbk->stepwise);
    }
    else {
        bbk->nratchets = 0;
        mpl_stepwise_init(s->stepw_type, bbk->numtaxa, s->hold, &bbk->stepwise);
    }
    
    // Unless using trees already in memory, clear the buffer
    if (s->stepw_type != MPL_AST_INMEM) {
        mpl_treelist_clear_all(s->treelist);
    }
    
    // Set up all the node buffers:

    bbk->srcs      = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    bbk->tgtslong  = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    bbk->tgtsshort = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    bbk->clips     = (mpl_node**)safe_calloc(bbk->num_nodes, sizeof(mpl_node*));
    
    return ret;
}

void mpl_bbreak_cleanup(mpl_bbreak* bbk)
{
    assert(bbk != NULL);
    
    bbk->numreps        = 0;
    bbk->num_rearrangs  = 0;
    bbk->nratchets      = 0;
    bbk->savelim        = 0;
    bbk->treelist       = NULL;
    bbk->doislandcheck  = true;
    
    safe_free(bbk->srcs);
    safe_free(bbk->tgtslong);
    safe_free(bbk->tgtsshort);
    safe_free(bbk->clips);
}

void mpl_bbreak_set_type(const mpl_bbreak_t bbkt, mpl_bbreak* bbk)
{
    assert(bbkt < MPL_BBREAK_MAX);
    bbk->bbktype = bbkt;
}

void mpl_do_bbreak(mpl_bbreak* bbk)
{
    // Set up all the global variables.
    // Iterate over the tree list.
    
//    mpl_bbreak_print_status_header(bbk);
    
    long i       = 0;
    long j       = 0;
    long nstarts = 0;
    
    time_t timein;
    time_t timeout;
    time_t tkeep = 0.0;
    
//    mpl_topol* current = NULL;
    mpl_topol* top     = NULL;
    
    mpl_tree* t = NULL;
    
    search_interrupt = 0;
    
    timein = (float)clock()/CLOCKS_PER_SEC;

    tkeep = timein;
    
    t = mpl_new_tree(bbk->numtaxa);
    
//    mpl_stepwise_init(MPL_AST_RANDOM, bbk->numtaxa, 1, &bbk->stepwise);

    bbk->shortest = MPL_MAXSCORE;//0.0;
    
//    mpl_bbreak_print_status_header(bbk);
    
    for (i = 0; i < bbk->numreps; ++i) {
        
        bbk->rep_ntrees = 0;
        bbk->repstart = NULL;
        
        printf("Replicate: %li\n", i+1);
        // If the buffer is empty get one or more trees by stepwise addition
        // Otheriwse, starttrees is the bbk buffer
        if (bbk->stepwise.astype != MPL_AST_INMEM) {
            mpl_stepwise_do_search(&bbk->stepwise);
        }

        // Then get the trees from the stepwise struct
        
        nstarts = 1; // For now, only using one start tree, but a 'swap on all'
                     // could be programmed in the future.
        
        for (j = 0; j < nstarts; ++j) {
            
            bbk->hitisland = false;
            
            if (i > 0) {
                // Add the last tree from the previous rep
                bbk->repstart = bbk->treelist->back;
                assert(bbk->repstart != NULL);
            }
            
            if (bbk->stepwise.astype != MPL_AST_INMEM) {
                // Add the first tree from the rep in the buffer
                top = mpl_treelist_get_shortest(bbk->stepwise.queued);
                mpl_tree_read_topol(t, top);
                mpl_tree_rebase(0, t);
                
                // Initiate a new replicate
                if (mpl_treelist_add_tree(true, t, bbk->treelist) != NULL) {
                    mpl_treelist_clear_back_to(bbk->repstart, bbk->treelist);
                    break;
                }
            }

            if (i == 0) {
                bbk->head = mpl_treelist_get_shortest(bbk->treelist);
                bbk->shortest = bbk->bestinrep = bbk->head->score;
            } else {
                bbk->head       = bbk->treelist->back;
                bbk->bestinrep  = bbk->head->score;
            }
            
            if (bbk->bestinrep < bbk->shortest) {
                bbk->shortest = bbk->bestinrep;
                mpl_treelist_clear_all(bbk->treelist);
                mpl_treelist_add_tree(false, t, bbk->treelist);
                bbk->head = bbk->treelist->trees;
                bbk->repstart = NULL;
            }
            
            bbk->hitisland = false;
            
            if (bbk->nratchets > 0) {
                mpl_do_ratchet_search(t, bbk);
            } else {
                mpl_swap_all(true, t, bbk); // Do the branch-swapping on this tree
            }

            // If no better equal or tree was found in this replicate,
            // wipe the rep
            if ((bbk->bestinrep > bbk->shortest)) {
                mpl_treelist_clear_back_to(bbk->repstart, bbk->treelist);
            }

            printf("\n\tReplicate completed.\n");
            printf("\tNumber of trees saved: %li\n",
                   bbk->treelist->num_trees); // Use arithmetic on indices
//            printf("\tNumber of hits to shortest tree: %i\n", bbk->nhits);
            if (bbk->treelist->num_trees > 0) {
                printf("\tShortest tree found: %.0f steps\n\n", bbk->bestinrep);
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
    
    printf("\n\nHeuristic search %s %li seconds:\n",
           compl_msg,
           timeout - timein);
    printf("\t%lu rearrangements tried.\n", bbk->num_rearrangs);
    printf("\tShortest tree found: %.3f steps\n", bbk->shortest);
    printf("\t%li trees saved\n", bbk->treelist->num_trees);
    
    mpl_delete_tree(&t);

}

void mpl_do_ratchet_search(mpl_tree* t, mpl_bbreak* bbk)
{
    int i = 0;
    
    double  oldbest = 0.0;
    double  iterbest = 0.0;
    long    oldsavelim = 0;
    
    printf("Initiating ratchet search\n");
    
    bbk->nhits = 0;
    
    // Nixon 1:
    if (bbk->shortest == 0) {
        bbk->shortest = t->score;
    }
    
    bbk->rep_ntrees = 1;
    oldsavelim = bbk->savelim;
    bbk->savelim = 1;
    
    bbk->bestinrep = t->score;
    bbk->hitisland = false;
    bbk->doislandcheck = true;
    
    // Swap the starting tree
    mpl_swap_all(false, t, bbk);
    oldbest = bbk->bestinrep;
    iterbest = oldbest;
    bbk->ratchhead = bbk->treelist->back;
    
    // Nixon 2:
//    mpl_tree_read_topol(t, bbk->treelist->back);
    //bbk->rep_ntrees = 0;
    
    if (search_interrupt == 1) {
        return;;
    }

    for (i = 0; i < bbk->nratchets; ++i) {
        
        printf("\r                                                          ");
        printf("\r\tRatchet iteration %i. Best tree: %.0f steps",
               i + 1,
               bbk->shortest);
        fflush(stdout);

        bbk->hitisland = false;

        // Nixon 3: ============================================================
        mpl_do_ratchet_weights();

        // Nixon 4: ============================================================
        // Swap the tree with new weights
       
        // Switch these for running a perturbed search
        bbk->doislandcheck = false;
        bbk->bestinrep = MPL_MAXSCORE;
        
        bbk->head = bbk->ratchhead;//bbk->treelist->back;
        mpl_tree_read_topol(t, bbk->head);
        mpl_treelist_add_tree(true, t, bbk->treelist);
        //bbk->rep_ntrees = 0;
        bbk->savelim = 1;
        mpl_swap_all(false, t, bbk);

        bbk->doislandcheck = true;
        
        // Nixon 5; ============================================================
        mpl_reset_std_weights();

        // Restore that tree
        mpl_tree_read_topol(t, bbk->treelist->back);
        mpl_treelist_clear_back_to(bbk->ratchhead, bbk->treelist);
        // Get the current best score?
        // Reset head to last tree in buffer
        bbk->head = bbk->treelist->back;
        bbk->head->score = t->score = mpl_length_only_parsimony(MPL_MAXSCORE, t);
        bbk->bestinrep = t->score;
        
        // Swap the tree
        bbk->rep_ntrees = 0;
        mpl_swap_all(false, t, bbk);
        
        // Best tree for this iteration:
        iterbest = bbk->bestinrep;
        
        if (iterbest <= oldbest) {
            bbk->ratchhead = bbk->treelist->back;
        } else {
            mpl_treelist_clear_back_to(bbk->ratchhead, bbk->treelist);
        }

        if (bbk->bestinrep < oldbest) {
            oldbest = bbk->bestinrep;
        }
        
        if (search_interrupt == 1) {
            break;
        }
        // Nixon 6 =============================================================
    }

    bbk->bestinrep = oldbest;
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

/*
 Performs all branch-swapping operations on the supplied tree, either using
 SPR or TBR as set in the mpl_bbreak struct supplied.
 */
void mpl_branch_swap(mpl_tree* t, mpl_bbreak* bbk)
{
    
    long        clipmax = 0; // Limit for number of clippings to perform
    long        tgtnum  = 0; // Limit for number of reconnection target sites
    double      score   = 0.0;  // Local storage for tree score
//    double      diff    = 0.0;
    mpl_node*   left    = NULL;
    mpl_node*   right   = NULL;
    mpl_node*   csite   = NULL;
    mpl_node*   rtlef   = NULL;
    mpl_node*   rtrig   = NULL;
    mpl_node**  clips   = NULL;
    mpl_node**  srcs    = bbk->srcs;
    mpl_node**  src     = srcs;
    mpl_node**  tgts    = NULL;
    long        i       = 0;    // Iterator
    long        j       = 0;    // Iterator
    
    mpl_tree_traverse(t); // Traverse the tree and get all nodes in the tree
    clips = bbk->clips;
    
    // TODO: Might be good to use the tips first; then the postorder list of internal nodes
    // TODO: Doing this could allow you to minimize times the buffers are copied back
    for (i = 1, clipmax = 0; i < t->size; ++i) {
        if (&t->nodes[i] != t->base && t->nodes[i].anc != t->base) {
            clips[clipmax] = &t->nodes[i];
            ++clipmax;
        }
    }

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
        left  = NULL;
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
        
        // Assign an absolute minimum score in each parsimony partition
        // Restore this if using the break in the shortcut algorithm
        mpl_scoretree_calc_abs_minscores();
        
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
            
            // The first time around, the subtree won't get rerooted, so only
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
                
                double limit = -1.0;
                if (bbk->bestinrep == 0.0) {
                    limit = -1.0;
                }
                else {
                    limit = bbk->bestinrep;
                }
                
                score = mpl_score_try_parsimony(tgtlen + srclen,
//                                                -1.0,
                                                bbk->bestinrep,
                                                clips[i],
                                                tgts[j],
                                                t);

                t->score = (score + tgtlen + srclen);
//                t->score = mpl_fullpass_parsimony(t);
//                t->score = mpl_length_only_parsimony(MPL_MAXSCORE, t);

                if (t->score <= bbk->bestinrep) {
                    if (t->score < bbk->bestinrep) {

                        bbk->hitisland   = false;
                        bbk->foundbetter = true; // Either better overall or for rep
                        
                        if (t->score < bbk->shortest) {
                            bbk->shortest   = t->score;
                            mpl_treelist_clear_all(bbk->treelist);
                            bbk->repstart = NULL;

                        } else {
                            mpl_treelist_clear_back_to(bbk->repstart, bbk->treelist);
                        }
                        bbk->bestinrep = t->score;
                        
                        mpl_topol* ret = 0;
                        clips[i]->clipmark = false;
                        ret = mpl_treelist_add_tree(bbk->doislandcheck,
                                                    t,
                                                    bbk->treelist);
                        bbk->head = bbk->treelist->back;
                        clips[i]->lock = false;
                        if (ret != NULL && bbk->doislandcheck == true) {
                            if (t->score == bbk->shortest) {
                                bbk->hitisland = true;
                                bbk->rep_ntrees = 0;
                                return;
                            }
                        } else {
                            bbk->rep_ntrees = 1;
                        }
                        return;
                    } else if (bbk->savelim > 0) {
                        if (bbk->rep_ntrees < bbk->savelim) {
                            mpl_treelist_add_tree(true, t, bbk->treelist);
                            ++bbk->rep_ntrees;
                        }
                    } else {
                        mpl_treelist_add_tree(true, t, bbk->treelist);
                        ++bbk->rep_ntrees;
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
static void mpl_bbreak_print_status_header(mpl_bbreak* bbk)
{
    printf("\n\n");
    printf("               Swapping             Trees            Shortest        Shortest\n");
    printf("Replic.        tree no.             saved            this rep.       overall\n");
    printf("--------------------------------------------------------------------------------\n");
}

static void mpl_bbreak_print_status(long rep, mpl_bbreak* bbk)
{
    printf(" %li\t\t\t\t%li\t\t\t\t%li\t\t\t\t%.0f\t\t\t\t%.0f\n",
           rep+1,
           bbk->treelist->back->index,
           bbk->treelist->num_trees,
           bbk->bestinrep,
           bbk->shortest);
}

/**
 Swap all trees in memory from wherever the bbreak struct has had its head
 
 @param report Boolean value indicating whether to print report on search states
 @param t Pointer to an instance of the initial tree
 @param bbk Pointer to the bbreak record
 */
static void mpl_swap_all(const bool report, mpl_tree* t, mpl_bbreak* bbk)
{
    do {
        bbk->foundbetter = false;
        if (report == true) {
            printf("\r                                                                              ");
            printf("\r\tShortest tree found: %.0f; swapping %li of %li trees saved.",
                   bbk->bestinrep,
                   bbk->head->index+1,
                   bbk->treelist->num_trees);
            
            fflush(stdout);
        }

        mpl_branch_swap(t, bbk);

        if (bbk->foundbetter) {
            continue;
        }

        bbk->head = bbk->head->next;
        if (bbk->head != NULL) {
            mpl_tree_read_topol(t, bbk->head);
        }
        
        if (search_interrupt == 1) {
            break;
        }
    } while (bbk->head != NULL);
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
    
    mpl_node* n1   = tgtnbr;
    mpl_node* n2   = tgtnbr->anc;
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
    
    // TODO: This crap is not safe:
    mpl_node* start = NULL;
    start = mpl_node_get_sib(&t->nodes[0]); // The 0 assumption
    
    if (start == NULL) {
        bbk->nshorttgts = 0;
        bbk->nlongtgts  = 1;
        bbk->tgtslong[0] = &t->nodes[0];
        return 1;
    }
    
    bbk->nshorttgts = 0;
    bbk->nlongtgts  = 0;
    
    // Collect all the target sites in the target subtree
    mpl_bbreak_trav_targets(start,
                            site,
                            bbk->tgtslong,
                            bbk->tgtsshort,
                            &bbk->nlongtgts,
                            &bbk->nshorttgts);
    
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
