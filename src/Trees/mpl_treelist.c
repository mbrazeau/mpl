//
//  mpl_treelist.c
//  Morphy
//
//  Created by Martin Brazeau on 09/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#include "mpl_utils.h"
#include "mpl_treelist.h"

static int mpl_treelist_resize(const long num_taxa, const long extension, mpl_treelist* tl);
void mpl_treelist_extend(const long nelems, mpl_treelist* tl);
mpl_topol* mpl_treelist_pool_fetch(mpl_treelist* tl);


// TODO: Treelist should be indifferent to num_taxa
mpl_treelist* mpl_treelist_new(const long num_taxa, const long max_trees, const long increase_rate)
{
    int i = 0;
    mpl_treelist* tl = NULL;
    mpl_topol* p = NULL;
    mpl_topol* q = NULL;
    
    if ((tl = (mpl_treelist*)safe_calloc(1, sizeof(mpl_treelist)))) {
        tl->num_taxa = num_taxa;
        tl->num_trees = 0;
        tl->max_trees = max_trees;
        tl->increase_rate = increase_rate;
        tl->rep_num_trees = 0;
        
        q = p = mpl_topol_new(num_taxa);
        if (p == NULL) {
            printf("Warning: insufficient memory for requested treelist\n");
            return -1;
        }

        // Run the factory
        i = 1;
        while (i < tl->max_trees) {
            p->next = mpl_topol_new(num_taxa);
            if (p->next == NULL) {
                printf("Warning: insufficient memory to complete requested treelist capacity\n");
                break;
            }
            p = p->next;
            ++i;
        }
        
        // Push these to the front of the pool
        if (tl->pool != NULL) {
            p->next = tl->pool;
        }
        
        tl->pool = q;
    }
    
    return tl;
}

void mpl_treelist_delete(mpl_treelist** tl)
{
    mpl_treelist* tli = NULL;
    tli = *tl;
    if (tli != NULL) {
        
        if (tli->trees != NULL) {
            // TODO: Call new fxn mpl_topol_destroy_linked(tli->trees);
            tli->trees = NULL;
        }
        if (tli->pool != NULL) {
            // TODO: Call new fxn mpl_topol_destroy_linked(tli->pool);
            tli->pool = NULL;
        }
    }
}


// TODO: Re-write this
// TODO: Test this
void mpl_treelist_set_maxtrees(const long maxtrees, mpl_treelist* tl)
{
    if (maxtrees < tl->num_trees) {
        // TODO: Decide what to do here when maxtrees is set to something
        printf("WARNING: Reducing buffer size after allocation not implemented.\n");
        // less than what's currently stored/allowed. Possibly ignore this.
    }
    
    if (maxtrees > tl->max_trees) {
        mpl_treelist_resize(tl->num_taxa, maxtrees - tl->max_trees, tl);
        assert(tl->max_trees == maxtrees);
    }
    
//    printf("Resize tree list to %lu trees for a total of: ", tl->max_trees);
}

mpl_topol* mpl_treelist_add_tree(const bool checkdupes, mpl_tree* t, mpl_treelist* tl)
{
    int idx = 0;
    
    // First check if there's enough room
    if (tl->num_trees >= tl->max_trees) {
       
        // If not, check if an increase in the buffer size is allowed.
        if (tl->increase_rate == 0) {
            return NULL;
        }
        
        // TODO: Check return of this
        mpl_treelist_resize(t->num_taxa, tl->increase_rate, tl);
    }

    mpl_topol* top = mpl_treelist_pool_fetch(tl);
    if (top == NULL) {
        return NULL;
    }
    
    mpl_tree_record_topol(top, t);
    
    // Put the tree to the back of the list
    if (tl->back != NULL) {
        idx = tl->back->index+1;
        tl->back->next = top;
    } else {
        assert(tl->trees == NULL);
        tl->trees = top;
    }
    
    tl->back = top;
    tl->back->index = idx;
    top->next = NULL; // Sanity precaution
    ++tl->num_trees;

    assert(tl->num_trees <= tl->max_trees);
    
    // TODO: Rebase the topology if the tree is unrooted
    // TODO: This can only be done once you decide how to record topologies which may have less than the maximum number of tips...

    return NULL;
}


long mpl_treelist_get_numtrees(const mpl_treelist* tl)
{
    return tl->num_trees;
}

// TODO: This will need replacing
void mpl_treelist_overwrite_longest(mpl_tree* t, mpl_treelist* tl)
{
    long        i = 0;
    long        lcount = 0;
    double      biglen = 0.0;
    double      shortlen = 0.0;
    mpl_topol*  longest = NULL;
    mpl_topol*  p = NULL;
    
    
    if (tl->num_trees == tl->max_trees) {
        shortlen = tl->trees->score;
        p = tl->trees->next;
        while (p != NULL) {
            if (p->score > biglen) {
                biglen = p->score;
                longest = p;
                lcount = 1;
            } else if (p->score == biglen) { // <<<============================= USE ALMOST EQUAL
                ++lcount;
            } else if (p->score < shortlen) {
                shortlen = p->score;
            }
            p = p->next;
        }
    }
    
    if (t->score > biglen && biglen != 0.0 && tl->num_trees >= tl->max_trees) {
        return;
    }
    
    mpl_topol* top = longest;
    mpl_tree_record_topol(top, t);
    
    if (tl->num_trees == 0) {
        tl->back = top;
        ++tl->num_trees;
    }
}

mpl_topol* mpl_treelist_get_topol(const long tnum, mpl_treelist* tl)
{
    int i = 0;
    mpl_topol* p = NULL;
    
    if (tnum < tl->num_trees) {
        p = tl->trees;
        while (i < tnum && p != NULL) {
            p = p->next;
            ++i;
        }
    }

    return p;
}

// TODO: DELETE
void mpl_treelist_reset_head(mpl_treelist* tl)
{
//    tl->head = tl->front;
}

// TODO: DELETE
void mpl_treelist_reverse_head(mpl_treelist* tl)
{
    if (tl->head != NULL) {
/*        if (tl->head == tl->front) {
            tl->head = NULL;
        }
        else {
            --tl->head;
        }*/
    }
    else {
        tl->head = tl->back;
    }
}

// TODO: DELETE
mpl_topol* mpl_treelist_get_next(mpl_treelist* tl)
{
    mpl_topol* ret = NULL;
    
    if (tl->head != NULL) {
        
        ret = tl->head;
        
        if (tl->head == tl->back) {
            tl->head = NULL;
        }
        else {
            ++tl->head;
        }
    }
    
    return ret;
}

mpl_topol* mpl_treelist_get_shortest(mpl_treelist* tl)
{
    double shortest;
    
    mpl_topol* ret = NULL;
    mpl_topol* p = NULL;
    ret = tl->trees;
    shortest = ret->score;
    
    p = ret->next;
    while (p != NULL) {
        if (p->score < shortest) {
            shortest = p->score;
            ret = p;
        }
        p = p->next;
    }

    return ret;
}

void mpl_treelist_clear_all(mpl_treelist* tl)
{
    tl->back = tl->pool;
    if (tl->trees != NULL) {
        tl->pool = tl->trees;
    }
    tl->trees = NULL;
    tl->num_trees       = 0;
    tl->back            = NULL;

}

void mpl_treelist_clear_back_to(mpl_topol* head, mpl_treelist* tl)
{
    assert(head != NULL);
    
    if (head->next != NULL) {
        tl->back->next = tl->pool; // Connect back of list to front of pool.
        tl->pool = head->next; // Point pool to front of list segment being removed.
        head->next = NULL; // Cut segment off back of head.
        tl->back = head; // Reset the back of the list to the head.
        tl->num_trees = head->index + 1; // Reset the number of trees in the list.
    }
}

void mpl_treelist_clear_rep(mpl_treelist* tl)
{
    tl->num_trees = tl->num_trees - tl->rep_num_trees;
    tl->rep_num_trees = 0;
    assert(tl->num_trees >= 0);
    if (tl->num_trees > 0) {
        tl->back = &tl->trees[tl->num_trees-1];
    }
    else {
        tl->back = NULL;
    }
//    tl->head = NULL;
//    tl->back = tl->head;
}

mpl_topol* mpl_treelist_new_subrep(mpl_treelist* tl)
{
    // TODO: DELTE
//    tl->head = tl->repstart;
    tl->rep_num_trees = 0;
    
    return tl->head;
}

mpl_topol* mpl_treelist_newrep(bool checknew, mpl_tree* t, mpl_treelist* tl)
{
    // TODO: DELETE
    mpl_topol* ret = NULL;
    
    tl->rep_num_trees   = 0;
    
    ret = mpl_treelist_add_tree(checknew, t, tl);
    
    if (ret == NULL || checknew == false) {
//        tl->repstart = tl->head;
        tl->head = tl->back;
        tl->rep_index = tl->back->index;
        ret = tl->head;
    }
    else {
        ret = NULL;
    }
    
    return ret;
}



/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

// TODO: This really needs a return to check.
static int mpl_treelist_resize(const long num_taxa, const long extension, mpl_treelist* tl)
{
    int i = 0;
    int ret = 0;
    mpl_topol* p = NULL;
    mpl_topol* q = NULL;
    
    if (extension <= 0 || tl->increase_rate == 0) {
        return -1;
    }
    
    // Start the factory
    q = p = mpl_topol_new(num_taxa);
    if (p == NULL) {
        printf("Warning: insufficient memory for requested treelist extension\n");
        return -2;
    }
    
    ++tl->max_trees;
    
    // Run the factory
    while (i < extension) {
        p->next = mpl_topol_new(num_taxa);
        if (p->next == NULL) {
            printf("Warning: insufficient memory to complete requested treelist extension\n");
            ret = -3;
            break;
        }
        ++tl->max_trees;
        p = p->next;
        ++i;
    }
    
    // Push these to the front of the pool
    if (tl->pool != NULL) {
        p->next = tl->pool;
    }
    
    tl->pool = q;

    return ret;
}

mpl_topol* mpl_treelist_pool_fetch(mpl_treelist* tl)
{
    mpl_topol* ret = NULL;
    mpl_topol* p = NULL;
    
    if (tl->pool == NULL) {
        if (tl->increase_rate == 0) {
            return NULL;
        }
        mpl_treelist_resize(tl->num_taxa, tl->increase_rate, tl);
    }
    
    ret = tl->pool;
    p = ret->next;
    ret->next = NULL; // Pops the topology
    tl->pool = p;
    
    return ret;
}

//void mpl_treelist_push_back(mpl_topol* top, mpl_treelist* tl)
//{
//    if (tl->back != NULL) {
//        mpl_topol_link(tl->back, top);
//
//    }
//    else {
//        tl->front = top;
//        top->back = NULL;
//    }
//
//    tl->back = top;
//}

//void mpl_treelist_extend(const long nelems, mpl_treelist* tl)
//{
//    
//    long i = 0;
//    long limit = 0;
//    mpl_topol* nt = NULL;
//    mpl_topol* en = NULL;
//    
//    // Check if the requested number of elements will exceed maxtrees
//    // and if that maximum is allowed to be extended.
//    if ((tl->num_trees + nelems) > tl->max_trees) {
//        if (tl->increase_rate == 0) {
//            limit = tl->max_trees - tl->num_trees;
//        }
//    }
//    else {
//        limit = nelems;
//        tl->max_trees += nelems;
//    }
//    
//    // If the request amounts to zero, then do nothing.
//    if (!limit) {
//        return;
//    }
//    
//    // Set up the start of the linked list
//    nt = mpl_topol_new(tl->num_taxa);
//    
//    // Check the list isn't empty
//    if (tl->back != NULL) {
//        mpl_topol_link(tl->back, nt);
//    }
//    else {
//        assert(tl->front == NULL);
//        tl->back = tl->front = nt;
//    }
//    
//    do {
//        en = mpl_topol_new(tl->num_taxa);
//        mpl_topol_init(tl->num_taxa, en);
//        mpl_topol_link(nt, en);
//        nt = en;
//        en = en->next;
//        tl->back = nt;
//        ++i;
//    } while (i < nelems);
//    
//    assert(en == NULL);
//
//}
