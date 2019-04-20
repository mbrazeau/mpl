//
//  testmplnode.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <string.h>

#include "mpltest.h"
#include "testutils.h"
#include "testmplnode.h"
#include "../src/Trees/mpl_node.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "../src/Trees/mpl_newick_rdr.h"

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

int test_node_get_sibling (void)
{
    theader("Test getting sibling");
    
    int failn = 0;
    
    long numtaxa = 5;
    char* nwkstring = "((1,5),((2,4),3));";
    
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(numtaxa, &rdr);
    
    mpl_topol* top = mpl_topol_new(numtaxa);
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    mpl_newick_read(nwkstring, top, &rdr);
    
    mpl_tree_read_topol(t, top);
    
    mpl_node* sib1 = NULL;
    mpl_node* sib2 = NULL;
    mpl_node* n = &t->nodes[0];
    
    sib1 = mpl_node_get_sib(n);
    sib2 = mpl_node_get_sib(&t->nodes[0]);
    
    if (sib1 != sib2) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (sib1 != &t->nodes[4]) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    sib1 = mpl_node_get_sib(&t->nodes[1]);
    
    if (sib1 != &t->nodes[3]) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (mpl_node_get_sib(mpl_node_get_sib(sib1)) != sib1) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_topol_delete(&top);
    
    return failn;
}

int test_swap_parent_child (void)
{
    theader("Test swapping parent and child nodes");
    
    int failn = 0;
    
    long nnodes = 4;
    mpl_node* n, *l, *r, *p;
    p = n = r = l = NULL;
    
    mpl_node** ndptrs = (mpl_node**)safe_calloc(nnodes, sizeof(mpl_node*));
    
    int i = 0;
    for (i = 0; i < nnodes; ++i) {
        ndptrs[i] = mpl_new_node();
        ndptrs[i]->mem_index = i;
        if (i > 0) {
            ndptrs[i]->tip = i;
        }
    }
    
    n = ndptrs[0];
    p = ndptrs[1];
    l = ndptrs[2];
    r = ndptrs[3];
    
    mpl_node_push_desc(n, l);
    mpl_node_push_desc(n, r);
    mpl_node_push_desc(p, n);
    
    mpl_node_swap_anc_child(l, n);
    
    // TODO: Make this an actual test!
    
    return failn;
}

int test_binary_node_clip (void)
{
    theader("Test binary node clipping");
    
    int failn = 0;
    
    long numtaxa = 10;
    char* nwkstring = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    
    mpl_tree_read_topol(t, &top);
    
    char* nwkout = NULL;
    mpl_tree_write_newick(&nwkout, t);
    printf("Original tree: %s\n", nwkout);
    free(nwkout);
    
    // Clip
    mpl_node* clip  = NULL;
    mpl_node* cbase = NULL;
    mpl_node* csite = NULL;
    
    clip  = t->nodes[1].anc;
    cbase = clip->anc;
    
    csite = mpl_node_get_sib(clip);

    mpl_node_bin_clip(clip);

    if (mpl_tree_checker(t)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_tree_write_newick(&nwkout, t);
    printf("Clipped tree:  %s\n", nwkout);
    free(nwkout);
    
    // Rejoin
    mpl_node_bin_connect(NULL, csite, clip);
    
    // Test for problems with the tree
    if (mpl_tree_checker(t)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    // Print
    mpl_tree_write_newick(&nwkout, t);
    printf("Restored tree: %s\n", nwkout);
   
    
    if (strcmp(nwkstring, nwkout)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    free(nwkout);
    
    csite = mpl_node_get_sib(clip);
    
    mpl_node_bin_clip(clip);
    
    mpl_tree_write_newick(&nwkout, t);

    // Rejoin
    mpl_node_bin_connect(csite, NULL, clip);
    
    // Test for problems with the tree
    if (mpl_tree_checker(t)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    // Print
    mpl_tree_write_newick(&nwkout, t);
    printf("Rejoined to right: %s\n", nwkout);
    
    
    if (!strcmp(nwkstring, nwkout)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_topol_cleanup(&top);
    
    return failn;
}
