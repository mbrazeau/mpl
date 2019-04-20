//
//  testmpltree.c
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
#include "testmpltree.h"

// TODO: Remove/replace:
static void mpl_node_bin_traverse_temp(mpl_node* n)
{
    if (n->tip) {
        printf("%li", n->tip);
        return;
    }
    
    printf("(");
    mpl_node_bin_traverse_temp(n->left);
    printf(",");
    mpl_node_bin_traverse_temp(n->right);
    printf(")");
}

int test_tree_assembly_from_topology (void)
{
    theader("Test assembling tree from topology");
    
    int failn = 0;
    
    int numtaxa = 5;
    int numnodes = 2 * numtaxa - 1;
//    long edgetable[] = {5, 6, 7, 6, 5, 8, 7, 8, -1, -1};
//    long edgetable[] = {6, 8, 7, 8, 6, -1, 5, 5, 7, -1 };
    long edgetable[] =   {6, 8, 7, 8, 6, -1, 5, 5, 7, -1};
//    long edgetable[] = {7, 5, 6, 5, 7, 6, 8, 8, -1, -1};
    
    mpl_topol top;
    top.num_taxa = numtaxa;
    top.num_nodes = numnodes;
    top.lock = 0;
    top.edges = &edgetable[0];
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    mpl_tree_read_topol(t, &top);
    
    if (mpl_tree_checker(t)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_tests_tree_traverse(t);
    printf("\n");
    
    // TODO: Finish
    
    mpl_delete_tree(&t);
    
    return failn;
}

int test_binary_postorder (void)
{
    theader("Test binary postorder");
    
    int failn = 0;
    
    int numtaxa = 5;
    int numnodes = 2 * numtaxa - 1;
    long edgetable[] = {5, 6, 7, 6, 5, 8, 7, 8, -1, -1};
    long exp_order[] = {0, 4, 5, 2, 1, 3, 6, 7, 8};
    
    mpl_topol top;
    top.num_taxa = numtaxa;
    top.num_nodes = numnodes;
    top.lock = 0;
    top.edges = &edgetable[0];
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    mpl_tree_read_topol(t, &top);
    
    mpl_node_bin_traverse_temp(t->base);
    printf("\n");
    
    mpl_tree_traverse(t);
    
    if (mpl_tree_checker(t)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    int i = 0;
    for (i = 0; i < numnodes-1; ++i) {
        if (t->postord_all[i]->mem_index != exp_order[i]) {
            ++failn;
            pfail;
        }
        else {
            ppass;
        }
    }
    
    mpl_delete_tree(&t);
    return failn;
}


int test_polytomous_postorder (void)
{
    theader("Test traversal on polychotomous tree");
    
    int failn = 0;
    
    int numtaxa = 5;
    int numnodes = 2 * numtaxa - 1;
    long edgetable[] = {5, 6, 6, 6, 5, 7, 7, -1, -1, -1};
    
    mpl_topol top;
    top.num_taxa = numtaxa;
    top.num_nodes = numnodes;
    top.lock = 0;
    top.edges = &edgetable[0];
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    mpl_tree_read_topol(t, &top);
    
//    mpl_node_bin_traverse_temp(t->base);
    int i = 0, j = 0;
    mpl_node_poly_traverse(t->base, t, &i, &j);
    printf("\n");
    
    mpl_tree_traverse(t);
    
    if (mpl_tree_checker(t)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    //int i = 0;
    for (i = 0; i < numnodes-1; ++i) {
//        if (t->postord_all[i]->mem_index != exp_order[i]) {
//            ++failn;
//            pfail;
//        }
//        else {
//            ppass;
//        }
    }
    
    mpl_delete_tree(&t);
//    mpl_topol_cleanup(&top);
    
    return failn;
}

int test_worst_case_polytomy (void)
{
    theader("Testing worst-case polytomy");
    
    int failn = 0;
    
    int numtaxa = 15;
    char* newick = "(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_tree* t = NULL;
    t = mpl_new_tree(numtaxa);
    
    mpl_topol_reset(numtaxa, &top);
    mpl_newick_read(newick, &top, &nwkrdr);
    mpl_tree_read_topol(t, &top);
    mpl_tree_traverse(t);
   
    if (mpl_tree_checker(t)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    char* nwkprint = NULL;
    mpl_tree_write_newick(&nwkprint, t);
    printf("Result: %s\n", nwkprint);
    
    int res = 0;
    if ((res = strcmp(newick, nwkprint))) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_delete_tree(&t);
    mpl_topol_cleanup(&top);
    free(nwkprint);
    
    return failn;
}

int test_newick_writing (void)
{
    theader("Test Newick string writing");
    
    int failn = 0;
    
    long numtaxa = 5;
    char* nwkstring = "((1,5),((2,4),3));";
    
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
    
    printf("%s", nwkout);
    printf("\n");
    
    char* nwkprint = NULL;
    mpl_tree_write_newick(&nwkprint, t);
    printf("Result: %s\n", nwkprint);
    
    int res = 0;
    if ((res = strcmp(nwkstring, nwkprint))) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    free(nwkrdr.namebuffer);
    mpl_delete_tree(&t);
    mpl_topol_cleanup(&top);
    
    return failn;
}

int test_tree_rebasing (void)
{
    theader("Test rebasing of tree");
    
    int failn = 0;
    
    long numtaxa = 5;
    char* nwkstring = "((1,5),((2,4),3));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    
    mpl_tree_read_topol(t, &top);
    
    char* nwk = NULL;
    mpl_tree_write_newick(&nwk, t);
    printf("Original: %s\n", nwk);
    free(nwk);
    nwk = NULL;
    
    mpl_tree_rebase(0, t);
    
    nwk = NULL;
    mpl_tree_write_newick(&nwk, t);
    printf("Rebased:  %s\n", nwk);
    free(nwk);
    nwk = NULL;
    
    mpl_delete_tree(&t);
    mpl_topol_cleanup(&top);
    
    return failn;
}

int test_tree_rebasing_bigger_tree (void)
{
    theader("Test rebasing of 10-taxon tree");
    
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
    
    char* nwk = NULL;
    mpl_tree_write_newick(&nwk, t);
    printf("Original: %s\n", nwk);
    free(nwk);
    nwk = NULL;
    
    mpl_tree_rebase(9, t);
    
    nwk = NULL;
    mpl_tree_write_newick(&nwk, t);
    printf("Rebased:  %s\n", nwk);
    free(nwk);
    nwk = NULL;
    
    mpl_delete_tree(&t);
    mpl_topol_cleanup(&top);
    return failn;
}

int test_perform_all_rerootings_small (void)
{
    theader("Test perform all rerootings on a small tree");
    
    int failn = 0;
    
    const long numtaxa = 5;
    const long numnodes = 2 * numtaxa - 1;
    char* nwkstring = "((1,5),((2,4),3));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_node* sites[numnodes];
    sites[0] = t->base->left;
    sites[1] = t->base->right->left;
    sites[2] = t->base->right->right;
    
    int i = 0;
    int numsites = 3;
    char* nwk = NULL;
    
    for (i = 0; i < numsites; ++i) {
        
        mpl_tree_rebase(sites[i]->mem_index, t);
        
        if (!sites[i]->tip) {
            sites[numsites] = sites[i]->left;
            ++numsites;
            sites[numsites] = sites[i]->right;
            ++numsites;
        }
        
        mpl_tree_write_newick(&nwk, t);
        printf("Rerooting: %i: %s\n", i+1, nwk);
        free(nwk);
        nwk = NULL;
    }
    
    mpl_delete_tree(&t);
    mpl_topol_cleanup(&top);
    
    return failn;
}

int test_perform_all_rerootings_large (void)
{
    theader("Test perform all rerootings on a larger tree");
    
    int failn = 0;
    
    const long numtaxa = 10;
    const long numnodes = 2 * numtaxa - 1;
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
    
    mpl_node* sites[numnodes];
    mpl_node** s, **sn;
    s = sn = sites;
    sites[0] = t->base->left;
    sites[1] = t->base->right->left;
    sites[2] = t->base->right->right;
    
    sn = &sites[3];
    
    // And do it with pointers (probably faster?):
    int i = 0;
//    int numsites = 3;
    char* nwk = NULL;
    
    for (s = sites; s < sn ; ++s) {
        ++i;
        mpl_tree_rebase((*s)->mem_index, t);
        
        if (!(*s)->tip) {
            *sn = (*s)->left;
            ++sn;
            *sn = (*s)->right;
            ++sn;
//            sites[numsites++] = sites[i]->left;
//            sites[numsites++] = sites[i]->right;
        }
        
        mpl_tree_write_newick(&nwk, t);
        printf("Rerooting: %i: %s\n", i, nwk);
        free(nwk);
        nwk = NULL;
    }
    
    mpl_delete_tree(&t);
    mpl_topol_cleanup(&top);
    
    return failn;
}
