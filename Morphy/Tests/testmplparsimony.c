//
//  mpltestparsimony.c
//  Morphy
//
//  Created by Martin Brazeau on 28/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "mpltest.h"
#include "../src/Characters/mpl_matrix.h"
#include "../src/Characters/mpl_parsim.h"
#include "testutils.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "../src/Trees/mpl_newick_rdr.h"
#include "../src/Analysis/mpl_scoretree.h"
#include "testmplparsimony.h"

int test_basic_parsimony (void)
{
    theader("Test simple two-tip parsimony case");
    
    int failn = 0;
    
    long ntax = 2;
    long nchar = 10;
    
    char *rawmatrix =
    "0000000010\
     1111111111;";
    
    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
    mpl_matrix_apply_data(m);
    
    double len = 0.0;
    len = mpl_fitch_downpass(0, 1, 2, &m->parsets[0]);
    
    if (len != 9.0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    
    return failn;
}

int test_parsimony_on_tree (void)
{
    theader("Test standard Fitch parsimony for a whole tree");
    
    int failn = 0;
    
    long ntax = 10;
    long nchar = 10;
    
    char *rawmatrix =
    "1010312231\
    2130233203\
    3001210203\
    2131111202\
    3302222312\
    2231202332\
    1023203131\
    1123103001\
    2222213220\
    3203321302;";
    
    char* newick = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
    mpl_matrix_apply_data(m);
    
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_init(ntax, &top);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    mpl_newick_read(newick, &top, &rdr);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_tree_traverse(t);
    
    mpl_node* n = NULL;
    
    long i = 0;
    double len = 0.0;
    
    for (i = 0; i < ntax - 1; ++i) {
        n = t->postord_intern[i];
        len += mpl_fitch_downpass
        (n->left->mem_index, n->right->mem_index, n->mem_index, &m->parsets[0]);
    }
    
    if (len != 53.0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    mpl_delete_tree(&t);
    
    return failn;
}

int test_fullpass_parsimony (void)
{
    theader("Test full-pass Fitch parsimony");
    
    int failn = 0;
    
    long ntax = 10;
    long nchar = 10;
    
    char *rawmatrix =
    "1010312231\
    2130233203\
    3001210203\
    2131111202\
    3302222312\
    2231202332\
    1023203131\
    1123103001\
    2222213220\
    3203321302;";
    
    char* newick = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
    mpl_matrix_apply_data(m);
    
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_init(ntax, &top);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    mpl_newick_read(newick, &top, &rdr);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_tree_traverse(t);
    
    double len = 0.0;
    
    mpl_init_parsimony(m);
    
    len = mpl_fullpass_parsimony(t);
    
    if (len != 53.0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    mpl_delete_tree(&t);
    
    return failn;
}

int test_fullpass_with_inapplicables (void)
{
    theader("Test full-pass with inapplicable data");
    
    int failn = 0;
    
    long ntax = 12;
    long nchar = 1;
    
    char *rawmatrix =
    "01---1010101;";
    
    char* newick = "((((((1,2),3),4),5),6),(7,(8,(9,(10,(11,12))))));";
    
    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
    mpl_matrix_apply_data(m);
    
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_init(ntax, &top);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    mpl_newick_read(newick, &top, &rdr);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_tree_traverse(t);
    
    double len = 0.0;
    
    mpl_init_parsimony(m);
    
    len = mpl_fullpass_parsimony(t);
    
    if (len != 5.0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    mpl_delete_tree(&t);
    
    return failn;
}


int test_fullpass_with_multiple_inapplics (void)
{
    theader("Test full-pass with multiple characters with  data");
    
    int failn = 0;
    
    long ntax = 12;
    long nchar = 8;
    double expected[] = {5.0, 2.0, 3.0, 2.0, 1.0, 5.0, 5.0, 2.0};
    double explen = 0.0;
    
    long i = 0;
    for (i = 0; i < nchar; ++i) {
        explen = explen + expected[i];
    }
    
    char *rawmatrix =
    "2111-001\
     3-11-11?\
     --0----?\
     --0-----\
     11--1---\
     ?1--1-1?\
     ?1--100?\
     -1--111-\
     --1--00-\
     0-11-111\
     3-00-000\
     21001110;";
    
    char* newick = "((((((1,2),3),4),5),6),(7,(8,(9,(10,(11,12))))));";
    
    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
    mpl_matrix_apply_data(m);
    
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_init(ntax, &top);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    mpl_newick_read(newick, &top, &rdr);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_tree_traverse(t);
    
    double len = 0.0;
    
    mpl_init_parsimony(m);
    
    len = mpl_fullpass_parsimony(t);
    
    if (len != explen) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    mpl_delete_tree(&t);
    
    return failn;
}



















