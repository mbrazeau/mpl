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
#include "mpl_bitset.h"

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
    t->size = 0;
    t->nintern = 0;
    
    t->nodes = (mpl_node*)safe_calloc(t->num_nodes + 1, sizeof(mpl_node));

    // Set up desc buffer as having a minimum of 3 pointers to nodes
    for (i = 0; i < t->num_nodes + 1; ++i) {
        t->nodes[i].descs = (mpl_node**)safe_calloc(3, sizeof(mpl_tree*));
        // TODO: Need to find some way to test this
        if (t->nodes[i].descs == NULL) {
            mpl_delete_tree(&t);
            return NULL;
        }
        t->nodes[i].capacity = 2;
        t->nodes[i].ndescs = 0;
        t->nodes[i].mem_index = i;
        t->nodes[i].copy_index = -1;
        t->nodes[i].bipart = mpl_bitset_new(t->num_taxa);
        if (t->nodes[i].bipart == NULL) {
            mpl_delete_tree(&t);
            return NULL;
        }
        
        if (i < num_taxa) {
            t->nodes[i].tip = i + 1;
            mpl_bitset_set(i, t->nodes[i].bipart);
        }
    }
    
    t->nodes[t->num_nodes].tip = -1;
    t->dummy = &t->nodes[t->num_nodes];
    
    // Allocate buffers for postorder sets:
    t->postord_all = (mpl_node**)safe_calloc(t->num_nodes, sizeof(mpl_node*));
    t->postord_intern = (mpl_node**)safe_calloc(t->num_nodes - num_taxa,
                                                sizeof(mpl_node*));
    t->partial_pass = (mpl_node**)safe_calloc(t->num_nodes, sizeof(mpl_node*));
    return t;
}

int mpl_delete_tree(mpl_tree** t)
{
#ifdef DEBUG
    assert(t);
#endif
    
    if (t == NULL) {
        return -1;
    }
    
    if (*t == NULL) {
        return -2;
    }
    
    int i = 0;
    
    if ((*t)->nodes != NULL) {
        for (i = 0; i < (*t)->num_nodes + 1; ++i) {
            if ((*t)->nodes[i].descs != NULL) {
                free ((*t)->nodes[i].descs);
                (*t)->nodes[i].descs = NULL;
                mpl_bitset_delete(&(*t)->nodes[i].bipart);
            }
        }
        
        safe_free((*t)->nodes);
    }
    
    safe_free((*t)->postord_all);
    safe_free((*t)->postord_intern);
    safe_free((*t)->partial_pass);
    
    free(*t);
    *t = NULL;
    
    return 0;
}

int mpl_tree_read_topol(mpl_tree* t, mpl_topol* top)
{
#ifdef DEBUG
    assert(t && top);
#endif
    int ret = 0;
    long i = 0;
    long j = 0;
//    long k = 0;
    
    // Verify the inputs:
    if (top->num_nodes != t->num_nodes) {
        return -1;
    }

    mpl_tree_reset(t);
    // Hook up the descendants

    if (top->edges[top->num_nodes] < 0) {
        for (i = t->num_taxa; i < t->num_nodes; ++i) {
            
            j = 0;
            
            for (j = 0; j < t->num_nodes; ++j) {
               
                if (top->edges[j] == t->nodes[i].mem_index && t->nodes[j].anc == NULL)
                {
                    mpl_node_push_desc(&t->nodes[i], &t->nodes[j]);
                }
            }
            
            // If the node is polychotomous, indicate this on the tree
            if (t->nodes[i].ndescs > 2) {
                t->num_polys += t->nodes[i].ndescs;
            }
        }
    }
    else {
        
        // TODO: It may be possible to build any topology using this second
        // branch if some standards are applied to how topologies are recorded.
        
        // Build the nodes in order:
        // For each internal node in edges
        for (i = t->num_taxa; i < t->num_nodes; ++i) {
            // Get any node from the order list which has node i as parent
            for (j = 0; j < top->num_nodes; ++j) {
                long a = top->edges[top->num_nodes + j];
               
                // TODO: Consider another way to deal with this.
                // The current issue is that a topology that uses fewer than
                // top->num_nodes spaces will cause a crash or errors
                // Ultimately, it may be possible to avoid having to do this
                // a the top level of the loop, for instance by setting and
                // using the topology size
                if (a == -1)
                    continue;
                
                if (top->edges[a] == i) {
                    mpl_node_push_desc(&t->nodes[i], &t->nodes[a]);
                }
            }
            
            // If the node is polychotomous, indicate this on the tree
            if (t->nodes[i].ndescs > 2) {
                t->num_polys += t->nodes[i].ndescs;
            }
        }
    }
    
    if (top->lock > 0) {
        t->nodes[top->lock-1].lock = true;
    }
    
    // TODO: Make this more general and safer
    mpl_node* p = &t->nodes[0];
    i = 1;
    while (p->anc == NULL) p = &t->nodes[i++];
    while (p->anc != NULL) p = p->anc;
    t->base = p;
    t->dummy->left = p;
    p->anc = t->dummy;
    t->score = top->score;
    
    return ret;
}

int mpl_tree_record_topol(mpl_topol* top, mpl_tree* t)
{
#ifdef DEBUG
    assert(t);
#endif
    
    mpl_node* d = NULL;
    d = mpl_tree_dummy_root(t);
    
    mpl_tree_mark_uniquely(t);
    
    top->lock = 0; // TODO: This might need to be -1
    // Then copy into the topology record
    int i = 0;
    for (i = 0; i < t->num_nodes; ++i) {
        if (i < t->num_taxa) {
            if (t->nodes[i].anc != NULL) {
                top->edges[i] = t->nodes[i].anc->copy_index;
                if (t->nodes[i].clipmark == true) {
                    top->lock = t->nodes[i].copy_index + 1;
                }
            }
        } else {
            if (t->nodes[i].anc != NULL) {
                top->edges[t->nodes[i].copy_index] = t->nodes[i].anc->copy_index;
                
            }
        }
        
    }
    
    top->score = t->score;
    
//    mpl_topol_compress(top);
    
    return 0;
}

int mpl_tree_traverse(mpl_tree* t)
{
#ifdef DEBUG
    assert(t);
#endif
    // Count polytomies:
    // TODO: function here
    int i = 0, j = 0;
    if (t->num_polys) {
        mpl_node_poly_traverse(t->base, t, &i, &j);
    }
    else {
        mpl_node_bin_traverse(t->base, t, &i, &j);
    }
    
    // Set the size as indicated by the number of nodes in the tree
    t->size = i;
    t->nintern = j;
    
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
#ifdef DEBUG
    assert(t);
#endif
    
    int i = 0;
    
    for (i = 0; i <= t->num_nodes; ++i) {
        mpl_reset_node(&t->nodes[i]);
        if (i < t->num_taxa) {
            mpl_bitset_set(i, t->nodes[i].bipart);
        }
//        t->postord_all[i] = NULL;
//        if (i < t->num_taxa) {
//            t->postord_intern[i] = NULL;
//        }
    }

    t->root = NULL;
    t->size = 0;
    t->base = NULL;
    t->num_polys = 0;
    t->score = 0.0;
    
    return 0;
}

double mpl_tree_get_brlen(long br, mpl_tree* t)
{
#ifdef DEBUG
    assert(t);
#endif
    return t->nodes[br].length;
}

int mpl_tree_write_newick(char** dest, mpl_tree* t)
{
    mpl_str* nwk = NULL;
    
    if (!(nwk = mpl_str_new(t->num_taxa + t->num_nodes))) {
        return -1;
    }
    
    if (t->root != NULL) {
        // TODO: Append rooted signifier
    }
    else {
        // TODO: Append unrooted signifier
    }
    
    mpl_node_write_newick(nwk, t->dummy->left);
    mpl_str_append(';', nwk);
    // TODO: Make safer:
    *dest = mpl_str_c(nwk);
    
    // NOTE: Dirty trick:
    free(nwk); // Hands off pointer to allocated memory to *dest
    
    return 0;
}

int mpl_tree_rebase(long tgt, mpl_tree* t)
{
    if (t->nodes[tgt].anc == t->base) {
        return 1;
    }
    
    mpl_node *n = NULL;
    mpl_node *p = NULL;
    mpl_node *q = NULL;
//    mpl_node *r = NULL;
    
    mpl_node* n1 = &t->nodes[tgt];
    mpl_node* n2 = t->nodes[tgt].anc;
    
    n = n1->anc;
    p = n1;
    
    // Disconnect base from dummy. Otherwise, the dummy will end up inside the
    // tree. And we don't want that!
    t->base->anc = NULL;
    
    while (n) {
        q = n->anc;
        mpl_node_swap_anc_child(p, n);
        p = n;
        n = q;
    }
    
    // The old root has now been moved inside the tree with a dangling pointer.
    // This can now be extracted from the tree and moved back to where the
    // base of the tree should be.
    
    // TODO: BEGIN This is not safe for a polytomous node. All descendants of r
    // need to be pushed to the ancestor of r.
    mpl_node** r = NULL;
    q = t->base->left;
    
    // If q doesn't point to anything, find a descendant in the base's desc's
    // array that does.
    if (!q) {
        r = t->base->descs;
        while (!*r) {
            ++r;
        }
        q = *r;
    }
    assert(q);
    // TODO: END TODO
    
    // Now hook up the old descendants of the base to the ancestor of the base
    q->anc = t->base->anc;
    
    // Find the pointer to the base
    r = q->anc->descs;
    while(*r != t->base) {
        ++r;
    }
    *r = q;
    q->anc->left = q->anc->descs[0];
    q->anc->right = q->anc->descs[q->anc->ndescs-1];
    
    // Reset the base to the right place.
    mpl_node_clear_descs(t->base);
    mpl_node_push_desc(t->base, n1);
    mpl_node_push_desc(t->base, n2);

    // Put it back on the dummy:
    t->base->anc = t->dummy;
    t->dummy->left = t->base;
    
    return 0;
}

int mpl_root_tree(long tgt, mpl_tree* t)
{
    t->root = t->base;
    return mpl_tree_rebase(tgt, t);
}

int mpl_unroot_tree(mpl_tree* t)
{
    t->root = NULL;
    return 0;
}

inline mpl_node* mpl_tree_dummy_root(mpl_tree* t)
{
    return t->dummy;
}

void mpl_tree_calc_biparts(mpl_tree *t)
{
    mpl_node* n = NULL;
    mpl_node** p = NULL;
    
    int i = 0;
    for (i = 0; i < t->nintern; ++i) {
        n = t->postord_intern[i];
        p = n->descs;
        do {
            mpl_node_set_bipart((*p)->bipart, n->bipart);
            ++p;
        } while (*p);
    }
}

mpl_node* mpl_tree_bin_clip(mpl_node* n, mpl_tree* t)
{
    assert(!t->num_polys);
    
    mpl_node* s = NULL;
//    mpl_node* b = NULL;
    
    if (t->base->right == n) {
        s = n->anc;
    }
    
    s = mpl_node_bin_clip(n);
    
    if (s != NULL) {
        t->base = s;
        s->anc = t->base;
    }
    
    return s;
}

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

static inline void mpl_tree_reset_copy_indices(mpl_tree* t)
{
#ifdef DEBUG
    assert(t);
#endif
    
    int i = 0;
    
    for (i = 0; i <= t->num_nodes; ++i) {
        t->nodes[i].copy_index = -1;
    }
}

static void mpl_tree_mark_uniquely(mpl_tree* t)
{
    int i = 0;
    long index = t->num_taxa;
    mpl_node* p = NULL;
    mpl_node* d = mpl_tree_dummy_root(t);
    mpl_tree_reset_copy_indices(t);
    
    for (i = 0; i < t->num_taxa; ++i) {
        p = t->nodes[i].anc;
        if (p == NULL) {
            continue;
        }
        if (p->anc != d) {
            while (0 > p->copy_index) {
                p->copy_index = index;
                ++index;
                p = p->anc;
                if (p == d) break; // If the ancestor is the dummy,
                                        // you are at the bottom of the tree.
            }
        }
    }
}

mpl_node* mpl_tree_find_common
 (const mpl_node* p, const mpl_node* q, const mpl_tree *t)
{
    bool foundp = false;
    bool foundq = false;
    mpl_node* c = NULL;
    
    
    
    return c;
}
