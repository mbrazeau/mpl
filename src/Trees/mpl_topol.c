//
//  mpl_topol.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "mpl_topol.h"
#include "mpl_utils.h"


/*
 *  PUBLIC FUNCTION DEFINITIONS
 */

mpl_topol*  mpl_topol_new(long num_taxa)
{
    if (num_taxa == 0) {
        return NULL;
    }
    
    mpl_topol* newtop = NULL;
    
    newtop = (mpl_topol*)safe_calloc(1, sizeof(mpl_topol));
    
    if (newtop) {
        newtop->edges = NULL;
        newtop->newick = NULL;
        if (mpl_topol_init(num_taxa, newtop)) {
            // If this fails, then delete new topology and exit.
            mpl_topol_delete(&newtop);
            return newtop;
        }
    }
    
    return newtop;
}

int mpl_topol_delete(mpl_topol** top)
{
    if (!top) {
        return -1;
    }
    if (!*top) {
        return -1;
    }
    
//    safe_free((*top)->edges);
    mpl_topol_cleanup(*top);
    
    free(*top);
    *top = NULL;
    
    return 0;
}

/**
 Cleans up a topology record's memory. Should be used before freeing one or
 more instances of a topology that is not a pointer.
 
 @param top pointer to a topology record.
 @return 0 if success, non-zero if there's an error.
 */
int mpl_topol_cleanup(mpl_topol* top)
{
    safe_free(top->edges);
    safe_free(top->newick);
    return 0;
}

int mpl_topol_reset(long num_taxa, mpl_topol* top)
{
    if (top && num_taxa) {
        
        int i = 0;
        
//        top->num_polys = 0;
        top->root = -1;
        top->lock = -1;
        
        // TODO: At this point the function could check numtaxa
        // matches the old numtaxa and, if not, call back to
        // topol_initialise to fix it. This should not end up in an
        // infinite cycle.
        if (num_taxa != top->num_taxa) {
            mpl_topol_init(num_taxa, top);
        }
        
        top->num_taxa = num_taxa;
        top->num_nodes = 2 * num_taxa - 1;
        
        for (i = 0; i < 2 * top->num_nodes; ++i) {
            top->edges[i] = -1;
        }
        
//        top->next = NULL;
//        top->back = NULL;
//
        return 0;
    }
        
    return -1;
}

int mpl_topol_rebase(const long newb, mpl_topol* top)
{
    /* This function effectively re-roots the topology vector on the first
     * tip that corresponds to the virtual root or starting point of the tree.
     * It is essential to call this function on any topology that has been
     * rerooted (for instance, during branch-swapping) but is not rooted. This
     * allows comparison of unrooted topologies.
     */
    assert(newb < top->num_nodes);
    // TODO: Could move this to a separate manipulator environment
    // to avoid allocating/deallocating memory all of the time
    long i   = 0;
    long j   = 0;
    long an  = 0;
    long old = 0;
    long stanc = 0;
    long oldnb = 0;
    
    // Start ancestor is the current ancestor of the new base neighbor:
    stanc = top->edges[newb];
    
    // Find the current base of the tree:
    old = stanc;
    j = old;
    while (j != -1) {
        old = j;
        j = top->edges[j];
    }
    
    // Move "down" the edge list swapping ancestors to descendants
    i = top->edges[newb];
    j = newb;
    while (i != old) {
        // Store the ancestor of the current index:
        an = top->edges[i];
        
        // Make the previous index the ancestor of the current:
        top->edges[i] = j;
        
        // Store current index for the next iteration
        j = i;
        
        // Move to the old ancestor of the current node
        i = an;
    }
    
    // Find the neighbor of old that is not the new base
    for (i = 0; i < top->num_nodes; ++i) {
        if (top->edges[i] == old && i != newb) {
            oldnb = i;
        }
    }
    
    // Clean up the old root:
    top->edges[oldnb] = j;  // Parent of the old base neighbor is the
                            // penultimate node visited
    top->edges[newb] = old; // Parent of the new base is the original root
    top->edges[stanc] = old; // Parent of the old ancestor of the root is now
                             // the original root node
    
    return 0;
}

//int mpl_topol_link(mpl_topol* parent, mpl_topol* child)
//{
//    if (parent && child) {
//        
//        parent->next = child;
//        child->back = parent;
//        
//        return 0;
//    }
//    
//    return -1;
//}

int mpl_topol_compare(const mpl_topol* t1, const mpl_topol* t2)
{
    int i = 0;
    long int max = 0;
    if (t1->num_nodes != t2->num_nodes) {
        return -1;
    }
    
//    if (t1->compressed != t2->compressed) {
//        return 1;
//    }
    
    max = t1->num_nodes;
//    long* e1 = &t1->edges[0];
//    long* e2 = &t2->edges[0];
//    long* end = &t1->edges[t1->num_nodes-1];
//
//    while (e1 != end) {
//        if (*e1++ != *e2++) {
//            return 1;
//        }
//    }
//
    for (i = 0; i < max; ++i) {
        if (t1->edges[i] != t2->edges[i]) {
            return i+1;
        }
    }
    
    return 0;
}

int mpl_topol_init(long num_taxa, mpl_topol* top)
{
    int ret = 0;
    
    if (!top) {
        return -1;
    }
    
    top->lock = 0;
    top->num_taxa = num_taxa;
    top->num_nodes = 2 * num_taxa - 1;
    
    if (top->edges != NULL) {
        free(top->edges);
        top->edges = NULL;
    }
    
    top->edges = (long*)safe_calloc(2 * top->num_nodes, sizeof(long));
    if (!top->edges) {
        ret = -1;
    }
    
    mpl_topol_reset(num_taxa, top);
    
    return ret;
}


/**
 Copies the tree storage data from one topology record to another; copies the
 entire edges buffer inside the topology record of src into that buffer for
 dest. There is no change to linkages used in storing topology records in a
 list. This function does a limited check to see if the topology dimensions are
 the same. It will fail to copy if the two records have been sized for different
 numbers of taxa.

 @param src A pointer to the source topology record to be copied.
 @param dest A pointer to the destination topology record to be copied into.
 @return An integer indicating a return value: negative if failure and 0 for
 success.
 */
int mpl_topol_copy_data(const mpl_topol* src, mpl_topol* dest)
{
    if (src->num_taxa != dest->num_taxa) {
        return -1;
    }
    
    dest->num_nodes = src->num_nodes;
//    dest->num_polys = src->num_polys;
    dest->root      = src->root;
    dest->lock      = src->lock;
    dest->score     = src->score;
//    dest->compressed  = src->compressed;
    memcpy(dest->edges, src->edges, dest->num_nodes * sizeof(long));
    
    return 0;
}

int mpl_topol_destroy_linked(mpl_topol* firsttop)
{
    mpl_topol *p = firsttop;
    mpl_topol *q;
    while (p) {
        q = p->next;
        mpl_topol_delete(&p);
        p = q;
    }
    
    return 0; // TODO: Use error code
}

//void mpl_topol_compress(mpl_topol* top)
//{
//    long i = 0;
//
//    top->compressed = 0UL;
//
//    for (i = 0; i < top->num_nodes-1; ++i) {
//        // Compress fxn
//    }
//}

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */
