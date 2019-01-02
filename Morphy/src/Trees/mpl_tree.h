//
//  mpl_tree.h
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_tree_h
#define mpl_tree_h

#include "mpl_node.h"
#include "mpl_topol.h"

typedef struct _tree {
    
    long        num_taxa;
    long        num_nodes;
    long        tree_size;
    mpl_node*   nodes;
    mpl_node**  postord_all;
    mpl_node**  postord_intern;
    mpl_node*   base;
    mpl_node*   root;
    long        num_polys;
    
} mpl_tree;

#define mpl_get_intern_postord(tree, index) ((const long)(tree->postord_intern[index]->mem_index))
#define mpl_get_any_postord(tree, index)    ((const long)(tree->postord_all[index]->mem_index))
#define mpl_get_left(tree, index)  ((const long)(tree->postord_all[index]->left->mem_index))
#define mpl_get_right(tree, index) ((const long)(tree->postord_all[index]->right->mem_index))

mpl_tree*   mpl_new_tree(long num_taxa);
int         mpl_delete_tree(mpl_tree** t);
int         mpl_tree_read_topol(mpl_tree* t, mpl_topol* top);
int         mpl_record_topol(mpl_topol* top, mpl_tree* t);
int         mpl_tree_traverse(mpl_tree* t);
int         mpl_tree_push_desc(long tgt, long src, mpl_tree* t);
int         mpl_tree_reset(mpl_tree* t);

#endif /* mpl_tree_h */
