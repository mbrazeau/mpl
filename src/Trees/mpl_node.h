//
//  mpl_node.h
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_node_h
#define mpl_node_h

#include <stdlib.h>
#include <stdbool.h>
#include "mpl_utils.h"
#include "mpl_bitset.h"

typedef struct _node mpl_node;
typedef struct _tree mpl_tree;

typedef struct _node {
    
    mpl_node*   left;
    mpl_node*   right;
    mpl_node**  descs;
    long        tip;
    size_t      ndescs;
    size_t      capacity;
    mpl_node*   anc;
    mpl_bitset* bipart;
    long        mem_index;
    long        po_index;
    long        copy_index;
    long        weight;
    // NOTE: Any variables added should be reset in mpl_reset_node()
    char*       label; // Only points, no memory allocated by mpl_node.h functions
    double      length;
    bool        lock;
    bool        clipmark;
    bool        inpath;
    int         marked;
    int         x; // For tree drawing
    int         y; // For tree drawing
} mpl_node;

//#define mpl_node_get_sib(n) ((n)->anc->left == n) ? (n)->anc->right : (n)->anc->left

mpl_node*   mpl_new_node(void);
int         mpl_delete_node(mpl_node** n);
int         mpl_reset_node(mpl_node* n);
void        mpl_node_bin_traverse(mpl_node* n, mpl_tree* t, int* i, int* j);
void        mpl_node_poly_traverse(mpl_node* n, mpl_tree* t, int* i, int* j);
long        mpl_node_push_desc(mpl_node* tgt, mpl_node* src);
mpl_node*   mpl_node_pop_desc(mpl_node* n);
mpl_node*   mpl_node_remove_desc(mpl_node* desc);
long        mpl_node_insert_desc(mpl_node* n, mpl_node* desc, size_t at);
void        mpl_node_clear_descs(mpl_node* n);
void        mpl_node_write_newick(mpl_str* nwk, mpl_node* n);
mpl_node*   mpl_node_get_sib(mpl_node* n);
int         mpl_node_swap_anc_child(mpl_node* child, mpl_node *n);
void        mpl_node_rotate(mpl_node* n);
void        mpl_node_swap_desc(mpl_node* newdesc, mpl_node* olddesc);
mpl_node*   mpl_node_bin_clip(mpl_node* n);
void        mpl_node_bin_connect(mpl_node* toleft, mpl_node* toright, mpl_node* n);
void        mpl_node_collapse(mpl_node* n);
inline void mpl_node_lock(mpl_node* n);
inline void mpl_node_unlock(mpl_node* n);
inline bool mpl_node_islocked(const mpl_node* n);
void        mpl_node_set_bipart(mpl_bitset* desc, mpl_bitset* p);

#endif /* mpl_node_h */
