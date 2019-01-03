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
    long        mem_index;
    long        po_index;
    long        copy_index;
    long        weight;
    // NOTE: Any variables added should be reset in mpl_reset_node()
    char*       label; // TODO: Think of how to deal with label. Likely copy.
    double      length;
    
} mpl_node;

mpl_node*   mpl_new_node(void);
int         mpl_delete_node(mpl_node** n);
int         mpl_reset_node(mpl_node* n);
void        mpl_node_bin_traverse(mpl_node* n, mpl_tree* t, int* i, int* j);
void        mpl_node_poly_traverse(mpl_node* n, mpl_tree* t, int* i, int* j);
long        mpl_node_push_desc(mpl_node* tgt, mpl_node* src);
mpl_node*   mpl_node_pop_desc(mpl_node* n);
mpl_node*   mpl_node_remove_desc(mpl_node* desc);
long        mpl_node_insert_desc(mpl_node* n, mpl_node* desc, size_t at);

#endif /* mpl_node_h */