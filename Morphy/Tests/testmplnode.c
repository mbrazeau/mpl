//
//  testmplnode.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include "mpltest.h"
#include "testmplnode.h"
#include "../src/Trees/mpl_node.h"
#include "../src/Trees/mpl_tree.h"

int test_node_new_delete (void)
{
    theader("Test allocation and deletion of node");
    
    int failn = 0;
    
    mpl_node* n = NULL;
    
    n = mpl_new_node();
    
    if (!n) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_delete_node(&n);
    
    if (n) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    return failn;
}

int test_push_node (void)
{
    theader("Test pushing new descendant nodes to an ancestor");
    
    int failn = 0;
    
    long nnodes = 3;
    mpl_node** ndptrs = (mpl_node**)calloc(nnodes, sizeof(mpl_node*));
    
    int i = 0;
    for (i = 0; i < nnodes; ++i) {
        ndptrs[i] = mpl_new_node();
    }
    
    mpl_node_push_desc(ndptrs[0], ndptrs[1]);
    
    if (ndptrs[0]->left != ndptrs[0]->right) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_node_push_desc(ndptrs[0], ndptrs[2]);
    
    if (ndptrs[0]->left == ndptrs[0]->right) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (ndptrs[0]->ndescs != 2) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    for (i = 0; i < nnodes; ++i) {
        mpl_delete_node(&ndptrs[i]);
    }
    
    return failn;
}

int test_push_to_polynode (void)
{
    theader("Test pushing new descendant nodes making a polynode");
    
    int failn = 0;
    
    long nnodes = 5;
    mpl_node** ndptrs = (mpl_node**)calloc(nnodes, sizeof(mpl_node*));
    
    int i = 0;
    for (i = 0; i < nnodes; ++i) {
        ndptrs[i] = mpl_new_node();
        ndptrs[i]->mem_index = i;
    }
    
    for (i = 1; i < nnodes; ++i) {
        mpl_node_push_desc(ndptrs[0], ndptrs[i]);
    }
    
    if (ndptrs[0]->ndescs != 4) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    // Verify null-termination of the descs array
    if (ndptrs[0]->descs[ndptrs[0]->capacity] != NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    // Verify that null-termination is the last NULL node pointer
    if (ndptrs[0]->descs[ndptrs[0]->capacity-1] == NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }

    for (i = 0; i < nnodes; ++i) {
        mpl_delete_node(&ndptrs[i]);
    }
    
    return failn;
}

int test_pop_desc (void)
{
    theader("Test pop descednant");
    
    int failn = 0;
    
    long nnodes = 5;
    mpl_node** ndptrs = (mpl_node**)calloc(nnodes, sizeof(mpl_node*));
    
    long i = 0;
    for (i = 0; i < nnodes; ++i) {
        ndptrs[i] = mpl_new_node();
    }
    
    for (i = 1; i < nnodes; ++i) {
        mpl_node_push_desc(ndptrs[0], ndptrs[i]);
    }
    
    mpl_node* res = NULL;
    
    for (i = nnodes-1; i > 0; --i) {
        res = mpl_node_pop_desc(ndptrs[0]);
        if (ndptrs[0]->descs[ndptrs[0]->ndescs] != NULL) {
            ++failn;
            pfail;
        }
        else {
            ppass;
        }
        
    }
    
    if (ndptrs[0]->ndescs != 0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    for (i = 1; i < nnodes; ++i) {
        if (ndptrs[i]->anc != NULL) {
            ++failn;
            pfail;
        }
        else {
            ppass;
        }
    }
    
    for (i = 0; i < nnodes; ++i) {
        mpl_delete_node(&ndptrs[i]);
    }
    
    return failn;
}

int test_remove_descendant (void)
{
    theader("Test removing a descendant");
    
    int failn = 0;
    
    long nnodes = 5;
    mpl_node** ndptrs = (mpl_node**)calloc(nnodes, sizeof(mpl_node*));
    
    int i = 0;
    for (i = 0; i < nnodes; ++i) {
        ndptrs[i] = mpl_new_node();
        ndptrs[i]->mem_index = i;
    }
    
    for (i = 1; i < nnodes; ++i) {
        mpl_node_push_desc(ndptrs[0], ndptrs[i]);
    }
    
    // Remove from the 'middle'
    mpl_node* before = ndptrs[3];
    mpl_node* ret = NULL;
    ret = mpl_node_remove_desc(ndptrs[3]);
    
    if (ret != before) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (before == ndptrs[0]->descs[2]) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (ndptrs[0]->descs[2] != ndptrs[4]) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (ndptrs[0]->descs[2] == NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (ndptrs[0]->descs[3] != NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    for (i = 0; i < nnodes; ++i) {
        mpl_delete_node(&ndptrs[i]);
    }
    
    return failn;
}

int test_basic_bin_traversal (void)
{
    theader("Test binary traversal");
    
    int failn = 0;
    
    long nnodes = 3;
    mpl_node** ndptrs = (mpl_node**)calloc(nnodes, sizeof(mpl_node*));
    
    int i = 0;
    for (i = 0; i < nnodes; ++i) {
        ndptrs[i] = mpl_new_node();
        ndptrs[i]->mem_index = i;
        if (i > 0) {
            ndptrs[i]->tip = i;
        }
    }
    
    for (i = 1; i < nnodes; ++i) {
        mpl_node_push_desc(ndptrs[0], ndptrs[i]);
    }
    
    int j=0, k=0;
    
    mpl_tree* t = mpl_new_tree(3);
    
    mpl_node_bin_traverse(ndptrs[0], t, &j, &k);

    if (t->postord_intern[0] != ndptrs[0]) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (t->postord_all[0] != ndptrs[1]) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (t->postord_all[1] != ndptrs[2]) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (t->postord_all[2] != ndptrs[0]) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    // Do cleanup
    for (i = 0; i < nnodes; ++i) {
        mpl_delete_node(&ndptrs[i]);
    }
    
    free(ndptrs);
    mpl_delete_tree(&t);
    return failn;
}
