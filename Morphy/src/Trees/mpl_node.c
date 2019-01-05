//
//  mpl_node.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <string.h>
#include <assert.h>

// TODO: TEMPORARY:
#include <stdio.h>
#include "mpl_utils.h"
#include "mpl_node.h"
#include "mpl_tree.h"


/* Private function prototypes */

static inline void mpl_push_desc(mpl_node* tgt, mpl_node* src);
static void mpl_extend_desc_array(mpl_node* n, const size_t nelems);
static inline void mpl_update_left_right_ptrs(mpl_node* n);
static inline mpl_node** mpl_node_find_desc_ptr(const mpl_node* tgt);

/*
 *  PUBLIC FUNCTION DEFINITIONS
 */

mpl_node* mpl_new_node(void)
{
    mpl_node* new_node = NULL;
    
    new_node = (mpl_node*)safe_calloc(1, sizeof(mpl_node));
    
    new_node->descs = (mpl_node**)safe_calloc(3, sizeof(mpl_node*));
    new_node->capacity = 2;
    new_node->ndescs = 0;
    
    return new_node;
}

int mpl_delete_node(mpl_node** n)
{
#ifdef DEBUG
    assert(n);
#endif
    
    if ((*n)->descs) {
        free ((*n)->descs);
        (*n)->descs = NULL;
    }
    
    free(*n);
    *n = NULL;
    
    return 0;
}

int mpl_reset_node(mpl_node* n)
{
#ifdef DEBUG
    assert(n);
#endif

    // TODO: Deal with label
    n->left = NULL;
    n->right = NULL;
    memset(n->descs, 0, n->ndescs * sizeof(mpl_node*));
    n->ndescs = 0;
    // Don't touch n->capacity unless freeing the node
    n->anc = NULL;
    n->po_index = 0;
    n->copy_index = 0;
    n->weight = 0;
    
    return 0;
}

void mpl_node_bin_traverse(mpl_node* n, mpl_tree* t, int* i, int* j)
{
#ifdef DEBUG
    assert(n && t && i && j);
#endif
    if (n->tip) {
        t->postord_all[*i] = n;
        (*i)++;
        return;
    }
    
    mpl_node_bin_traverse(n->left, t, i, j);
    mpl_node_bin_traverse(n->right, t, i, j);
    
    t->postord_all[*i] = t->postord_intern[*j] = n;
    (*i)++;
    (*j)++;
}

void mpl_node_poly_traverse(mpl_node* n, mpl_tree* t, int* i, int* j)
{
#ifdef DEBUG
    assert(n && t && i && j);
    int _countcheck = 0;
#endif
    mpl_node** p = NULL;
    
    if (n->tip) {
        printf("%li", n->tip);
        t->postord_all[*i] = n;
        (*i)++;
        //printf(")");
        return;
    }
    
    printf("(");
    
    p = &n->descs[0];
    do {
        mpl_node_poly_traverse(*p, t, i, j);
        ++p;
#ifdef DEBUG
        if (*p) {
            printf(",");
        }

        ++_countcheck;
#endif
    } while (*p);
#ifdef DEBUG
    assert(_countcheck == n->ndescs);
#endif
    
    t->postord_all[*i] = t->postord_intern[*j] = n;
    (*i)++;
    (*j)++;
    
    printf(")");
}

long mpl_node_push_desc(mpl_node* tgt, mpl_node* src)
{
#ifdef DEBUG
    assert(tgt);
    assert(src);
    assert(tgt != src);
    assert(src->anc == NULL);
#endif
    
    if (tgt->ndescs < tgt->capacity) {
        
        mpl_push_desc(tgt, src);
    }
    else {
        mpl_extend_desc_array(tgt, (tgt->capacity + 1));
        mpl_push_desc(tgt, src);
    }
    
    assert(tgt->ndescs <= tgt->capacity);
    
    return tgt->ndescs;
}

mpl_node* mpl_node_pop_desc(mpl_node* n)
{
#ifdef DEBUG
    assert(n->ndescs != 0);
    assert(n->descs[0] != NULL);
#endif
    
    mpl_node* ret = NULL;
    
    if (n->ndescs > 0) {
        ret = n->descs[n->ndescs-1];
        ret->anc = NULL;
        n->descs[n->ndescs-1] = NULL;
        --n->ndescs;
    }
    
    return ret;
}

mpl_node* mpl_node_remove_desc(mpl_node* desc)
{
    mpl_node* ret = NULL;
    mpl_node** ancdescs = NULL;
    
    if (desc->anc != NULL) {
        
        ancdescs = desc->anc->descs;
        
        // Find the descendant itself
        while (*ancdescs != desc) ++ancdescs;
        ret = *ancdescs;
        
        // Move copy the remaining pointers over
        while (*(ancdescs+1) != NULL) {
            *ancdescs = *(ancdescs+1);
            ++ancdescs;
        }
        
        // Overwrite the last position
        *ancdescs = NULL;
        
        --desc->anc->ndescs;
        
        // Make sure the ancestor's left/right pointers are up-to-date:
        mpl_update_left_right_ptrs(desc->anc);
        
        desc->anc = NULL;
    }
    
    return ret;
}

void mpl_node_write_newick(mpl_str* nwk, mpl_node* n)
{
#ifdef DEBUG
    assert(n && nwk);
    int _countcheck = 0;
#endif
    mpl_node** p = NULL;
    
    if (n->tip) {
//        printf("%li", n->tip);
        mpl_str_append_int(n->tip, nwk);
        return;
    }
    
//    printf("(");
    mpl_str_append('(', nwk);
    
    p = &n->descs[0];
    do {
        mpl_node_write_newick(nwk, *p);
        ++p;
#ifdef DEBUG
        if (*p) {
//            printf(",");
            mpl_str_append(',', nwk);
        }
        
        ++_countcheck;
#endif
    } while (*p);
#ifdef DEBUG
    assert(_countcheck == n->ndescs);
#endif
    
//    printf(")");
    mpl_str_append(')', nwk);
}

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

static void mpl_extend_desc_array(mpl_node* n, const size_t nelems)
{
    assert(nelems > n->ndescs);
    mpl_node** res = NULL;
    res = (mpl_node**)safe_calloc(nelems + 1, sizeof(mpl_node*));
    memcpy(res, n->descs, (n->ndescs + 1) * sizeof(mpl_node*));
    free(n->descs);
    
    n->descs = res;
    
    n->capacity = nelems;
    
}

static inline void mpl_push_desc(mpl_node* tgt, mpl_node* src)
{
#ifdef DEBUG
    assert(tgt);
    assert(src);
    assert(src->anc == NULL);
    assert(tgt->capacity > tgt->ndescs);
    int _i = 0;
    while (tgt->descs[_i] != src && tgt->descs[_i]) ++_i;
    assert(tgt->descs[_i] != src);
#endif
    
    tgt->descs[tgt->ndescs] = src;
    src->anc = tgt;
    
    ++tgt->ndescs;
    mpl_update_left_right_ptrs(tgt);

    //tgt->descs[tgt->ndescs] = NULL;
}

static inline void mpl_update_left_right_ptrs(mpl_node* n)
{
    n->left = n->descs[0];
    n->right = n->descs[n->ndescs-1];
}

static inline mpl_node** mpl_node_find_desc_ptr(const mpl_node* tgt)
{
    mpl_node** ret = NULL;
    
    if (tgt->anc != NULL) {
        ret = tgt->anc->descs;
        while (*ret != tgt && ret) ++ret;
    }
    
    return ret;
}
