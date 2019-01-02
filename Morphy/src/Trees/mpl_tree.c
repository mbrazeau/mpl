//
//  mpl_tree.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//
#include <assert.h>

#include "mpl_tree.h"
#include "mpl_utils.h"

// Private function prototypes
static inline void mpl_tree_reset_copy_indices(mpl_tree* t);
static void mpl_tree_mark_uniquely(mpl_tree* t);

/*
 *  PUBLIC FUNCTION DEFINITIONS
 */

mpl_tree* mpl_new_tree(long num_taxa)
{
    long i = 0;
    mpl_tree* t = NULL;
    
    t = (mpl_tree*)safe_calloc(1, sizeof(mpl_tree));
    t->num_taxa = num_taxa;
    t->num_nodes = 2 * num_taxa - 1;
    t->tree_size = 0;
    
    t->nodes = (mpl_node*)safe_calloc(t->num_nodes, sizeof(mpl_node));

    // Set up desc buffer as having a minimum of 3 pointers to nodes
    for (i = 0; i < t->num_nodes; ++i) {
        t->nodes[i].descs = (mpl_node**)safe_calloc(3, sizeof(mpl_tree*));
        t->nodes[i].capacity = 2;
        t->nodes[i].ndescs = 0;
        t->nodes[i].mem_index = i;
        t->nodes[i].copy_index = -1;
        if (i < num_taxa) {
            t->nodes[i].tip = i + 1;
        }
    }
    
    // Allocate buffers for postorder sets:
    t->postord_all = (mpl_node**)safe_calloc(t->num_nodes, sizeof(mpl_node*));
    t->postord_intern = (mpl_node**)safe_calloc(t->num_nodes - num_taxa,
                                                sizeof(mpl_node*));
    
    return t;
}

int mpl_delete_tree(mpl_tree** t)
{
    
    if (t == NULL) {
        return -1;
    }
    
    if (*t == NULL) {
        return -2;
    }
    
    int i = 0;
    
    if ((*t)->nodes != NULL) {
        for (i = 0; i < (*t)->num_nodes; ++i) {
            if ((*t)->nodes[i].descs != NULL) {
                free ((*t)->nodes[i].descs);
                (*t)->nodes[i].descs = NULL;
            }
        }
        
        free((*t)->nodes);
        (*t)->nodes = NULL;
    }
    
    if ((*t)->postord_all) {
        free((*t)->postord_all);
        (*t)->postord_all = NULL;
    }
    
    if ((*t)->postord_intern) {
        free((*t)->postord_intern);
        (*t)->postord_intern = NULL;
    }
    
    free(*t);
    *t = NULL;
    
    return 0;
}

int mpl_tree_read_topol(mpl_tree* t, mpl_topol* top)
{
    int ret = 0;
    long i = 0;
    long j = 0;
    
    // Verify the inputs:
    if (top->num_nodes != t->num_nodes) {
        return -1;
    }
    
    // Hook up the descendants

    for (i = t->num_taxa; i < t->num_nodes; ++i) {

        j = 0;

        for (j = 0; j < t->num_nodes; ++j) {
            if (top->edges[j] == t->nodes[i].mem_index
                && t->nodes[j].anc == NULL)
            {
                mpl_node_push_desc(&t->nodes[i], &t->nodes[j]);
            }
        }
    }
    
    mpl_node* p = &t->nodes[0];
    while (p->anc) p = p->anc;
    t->base = p;
    
    return ret;
}

int mpl_record_topol(mpl_topol* top, mpl_tree* t)
{
    mpl_tree_mark_uniquely(t);
    
    // Then copy into the topology record
    int i = 0;
    for (i = 0; i < t->num_nodes; ++i) {
        if (t->nodes[i].anc != NULL) {
            top->edges[i] = t->nodes[t->nodes[i].anc->copy_index].copy_index;
        }
//        else {
//            top->edges[i] = -1;
//        }
    }
    
    return 0;
}

int mpl_tree_traverse(mpl_tree* t)
{
    // Count polytomies:
    // TODO: function here
    int i = 0, j = 0;
    if (t->num_polys) {
        // TODO: General traverse
    }
    else {
        
        mpl_node_bin_traverse(t->base, t, &i, &j);
    }
    
    return 0;
}

int mpl_tree_push_desc(long tgt, long src, mpl_tree* t)
{
    assert (tgt != src);
    mpl_node_push_desc(&t->nodes[tgt], &t->nodes[src]);
    return 0;
}

int mpl_tree_reset(mpl_tree* t)
{
    int i = 0;
    
    for (i = 0; i < t->num_nodes; ++i) {
        mpl_reset_node(&t->nodes[i]);
        t->postord_all = NULL;
        if (i < t->num_taxa) {
            t->postord_intern = NULL;
        }
    }
    
    t->root = NULL;
    t->tree_size = 0;
    t->base = NULL;
    t->num_polys = 0;
    
    return 0;
}

double mpl_tree_get_brlen(long br, mpl_tree* t)
{
#ifdef DEBUG
    assert(t);
#endif
    return t->nodes[br].length;
}

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

static inline void mpl_tree_reset_copy_indices(mpl_tree* t)
{
    int i = 0;
    
    for (i = 0; i < t->num_nodes; ++i) {
        t->nodes[i].copy_index = -1;
    }
}

static void mpl_tree_mark_uniquely(mpl_tree* t)
{
    int i = 0;
    long index = t->num_taxa;
    mpl_node* p = NULL;
    
    mpl_tree_reset_copy_indices(t);
    
    for (i = 0; i < t->num_taxa; ++i) {
        p = t->nodes[i].anc;
        if (p) {
            while (0 > p->copy_index) {
                p->copy_index = index;
                ++index;
                p = p->anc;
                if (!p) break;
            }
        }
    }
}
