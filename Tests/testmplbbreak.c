//
//  testmplbbreak.c
//  Morphy
//
//  Created by Martin Brazeau on 15/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//
#include <string.h>
#include "mpltest.h"
#include "testutils.h"

#include "testmplbbreak.h"
#include "testharddat.h"
#include "mpl_node.h"
#include "mpl_tree.h"
#include "mpl_topol.h"
#include "mpl_newick_rdr.h"
#include "mpl_matrix.h"
#include "mpl_scoretree.h"
#include "mpl_stepwise.h"

int test_bbreak_reroot_all_for_subtree (void)
{
    theader("Test rerooting of a subtree");
    
    int failn = 0;
    
    // Set up a tree
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
    printf("Original tree:\t\t\t%s\n", nwkout);
    free(nwkout);
    
    // Clip a 'non-trivial' branch
    mpl_node* src = NULL;
    mpl_node* clipsite = NULL;
    
    mpl_node* toleft = NULL;
    mpl_node* toright = NULL;
    //   Using ancestor of the ancestor of tip 2 which shoudl be the subtree
    //   ((2,7),(5,9))
    src = t->nodes[1].anc->anc;
    clipsite = mpl_node_get_sib(src);
    
    if (src->anc->left == clipsite) {
        toleft = clipsite;
    }
    else {
        toright = clipsite;
    }
    
    mpl_node* sites[10];
    mpl_node** s, **sn;
    
    s = sites;
    sites[0] = src->left;
    sites[1] = src->right->left;
    sites[2] = src->right->right;
    sn = &sites[3];
    
    for (s = sites; s < sn; ++s) {
        
        // Clip the node
        mpl_node_bin_clip(src);
        
        // Perform a rerooting
        mpl_test_bbreak_tbr_reroot(*s, src);
        
        if (!(*s)->tip) {
            *sn = (*s)->left;
            ++sn;
            *sn = (*s)->right;
            ++sn;
        }
        
        // Reconnect and print the tree
        mpl_node_bin_connect(toleft, toright, src);
        
        mpl_tree_write_newick(&nwkout, t);
        printf("With rerooted subtree:\t%s\n", nwkout);
        free(nwkout);
        nwkout = NULL;
        
        if (mpl_tree_checker(t)) {
            ++failn;
            pfail;
        }
        else {
            ppass;
        }
    }


    return failn;
}

int test_bbreak_store_allswaps (void)
{
    theader("Test making a list of all SPR rearrangements");
    
    int failn = 0;
    
    // Set up a tree
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
    mpl_tree_rebase(0, t);
    
    char* nwkout = NULL;
    mpl_tree_write_newick(&nwkout, t);
    printf("Original tree:\t\t\t%s\n", nwkout);
    free(nwkout);
    
    // Declare and initialise a bbreak struct
    // For now: manually set parameters in the search and bbreak structs
    mpl_search s;
    s.treelist = mpl_treelist_new(10, 1000, 0);
    
    s.bbreak_type = MPL_SPR_T;
    
    mpl_bbreak bbk;
    // The number of nodes in the bbreak will be the size of the tree
    mpl_tree_traverse(t);
    bbk.num_nodes = t->size;
    // Now the bbkreak struct can be initialised
    mpl_bbreak_init(&s, &bbk);
    
    mpl_test_branch_swap(t, &bbk);
    
    int i = 0;
    for (i = 0; i < bbk.treelist->num_trees; ++i) {
        mpl_tree_read_topol(t, &bbk.treelist->trees[i]);
        mpl_tree_write_newick(&nwkout, t);
        printf("TREE Morphy_%i\t=\t[&U] %s\n", (i + 1), nwkout);
        free(nwkout);
    }
    
    return failn;
}


int test_bbreak_store_TBR_allswaps (void)
{
    theader("Test making a list of all SPR rearrangements");
    
    int failn = 0;
    
    // Set up a tree
    long numtaxa = 24;
    char* nwkstring = "(1,(((((2,3),6),(12,((7,(8,9)),(10,11)))),4),((19,((20,21),(((13,5),(14,15)),(16,(17,18))))),(22,(23,24)))));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    mpl_tree_read_topol(t, &top);
    mpl_tree_rebase(0, t);
    
    char* nwkout = NULL;
    mpl_tree_write_newick(&nwkout, t);
    printf("Original tree: %s\n", nwkout);
    free(nwkout);
    
    // Declare and initialise a bbreak struct
    // For now: manually set parameters in the search and bbreak structs
    mpl_search s;
    s.treelist = mpl_treelist_new(numtaxa, 20000, 0);
    
    s.bbreak_type = MPL_TBR_T;
    
    mpl_bbreak bbk;
    memset(&bbk, 0, sizeof(mpl_bbreak));
    // The number of nodes in the bbreak will be the size of the tree
    mpl_tree_traverse(t);
    bbk.num_nodes = t->size;
    // Now the bbkreak struct can be initialised
    mpl_bbreak_init(&s, &bbk);
    
    mpl_test_branch_swap(t, &bbk);
    
//    int i = 0;
//    for (i = 0; i < bbk.treelist->num_trees; ++i) {
//        mpl_tree_read_topol(t, &bbk.treelist->trees[i]);
//        mpl_tree_write_newick(&nwkout, t);
//        printf("TREE Morphy_%i\t=\t[&U] %s\n", (i + 1), nwkout);
//        free(nwkout);
//    }
    
    printf("\nTBR complete. %li rearrangements tried\n", bbk.num_rearrangs);
    
    mpl_treelist_delete(&s.treelist);
    
    return failn;
}


int test_single_rep_hsearch (void)
{
    theader("Test single-replicate heuristic search");
    
    int failn = 0;

    int ntax = 23;
    int nchar = 57;

    int hold = 5; // Hold up to 3 trees
    
    char* rawmatrix =

// Vinther
   "1010?10?0-0000000000000000000000000000-00-00-00000-0000-0\
    111111110-111?010111111111111?10000000-00-00-00000-0000-0\
    111111110-1111010111111011101010000000-00-00-00000-0000-0\
    111111110-1110010111100000001000000000-00-00-00000-0000-0\
    111111110-1111110111111111111010000000-00-00-00000-0000-0\
    111111110-1111010111?00000001000000000-00-00-00000-0000-0\
    1110111111?000000000000000000001110001010000-00000-000?-0\
    1110111111?000000000000000000001111001010000-00000-0001-0\
    111001111110000000000000000000012210010110010000002010111\
    ?1???11?100?00000000001000000001221000-00-0100000000001-0\
    100000000-1100000000000000000000000110-00-011001001000020\
    100000000-1100000000000000000000000110-00-012101001000020\
    ??????????1?????011??????????????00000-00-011000103301010\
    ????????????????1?1?????????????????00-00-011000103301010\
    ????????????????1?1?????????????????00-00-011000113301010\
    ??????????1??01?011?101000000000000000-00-00-00000-0000-0\
    ??????????????0?000?000?000000?????00101?11100000020?0?1?\
    ??????????????0?000?000?000000?????00101?1110000102210111\
    ??????????????0?000?000?000000???110010100010000002000100\
    ??????????????0?000?000?000000?????001011101??000010?000?\
    ??????????1???0?000?000?000000010??0011?1?00-00000-0000-0\
    ??????????????0?000?000?000000?????001??1101???000000000?\
    ??????????????0?000?000000000001021000-00-00-00000-0000-0;";


    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_set_nnodes(2 * ntax, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
    mpl_matrix_set_gap_handle(GAP_MISSING, m);
    mpl_matrix_apply_data(m);
    mpl_init_parsimony(m);
    
    mpl_search s;
    s.use_ratchet = false;
    s.treelist = mpl_treelist_new(ntax, 100000, 0);
    s.num_taxa = ntax;
    s.ratch_iter = 0;
    s.bbreak_type = MPL_TBR_T;
    
    mpl_bbreak bbk;
    memset(&bbk, 0, sizeof(mpl_bbreak));
    
    // The number of nodes in the bbreak will be the size of the tree
    bbk.num_nodes = 2 * ntax-1;
    // Now the bbkreak struct can be initialised
    mpl_bbreak_init(&s, &bbk);
    bbk.bbktype = MPL_TBR_T;
    bbk.numreps = 1;
    bbk.nratchets = 0;
    bbk.savelim = 0;
//    bbk.bbktype = MPL_SPR_T;
    mpl_rng_set_seed(4);
    mpl_stepwise_init(MPL_AST_RANDOM, bbk.numtaxa, 1, &bbk.stepwise);
    mpl_do_bbreak(&bbk);
    
    mpl_tree* t  = mpl_new_tree(ntax);
    mpl_topol* top = NULL;
    char* nwkresult = NULL;
    int i = 0;
    
    for (i = 0; i < bbk.treelist->num_trees; ++i) {
        top =  mpl_treelist_get_topol(i, bbk.treelist);
        mpl_tree_read_topol(t, top);

        mpl_tree_write_newick(&nwkresult, t);
        printf("TREE Morphy_%i = [&U] %s [length: %.0f]\n", i+1, nwkresult, top->score);
        free(nwkresult);
    }
    
    if (bbk.shortest != 78.0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (bbk.treelist->num_trees != 66) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    mpl_treelist_delete(&s.treelist);
    
    return failn;
}


int test_mutliple_replicate_heuristic_search (void)
{
    theader("Test multiple addition sequence searches");
    
    int failn = 0;

    int ntax = 23;
    int nchar = 126;
    
    int hold = 10; // Hold up to 3 trees
    
    char* rawmatrix =
    "00010100100011011010011100020000-000011000101000111011?1-110?11010??0010000000100400000-02301?10001010101001000000010111101010\
    111?10-0101011001110000120021101100101000100001-100-10000000011011302011000001--1000000-02210000010201-00010-10100011000000000\
    100-01001?000-011110101021021110-00000000001-101010?11201001?10010310110211010112111100-?21000010?100000010110001010-011101110\
    00010100100010011111011100020000-000011000101?0?11101101-011?110?03300100000?0100400000-02301?10011000101001000000010111101010\
    111010-010000-00?000000120020000-00001000001-01-110210000000101011412000001110111000010-0121010101000000000100010000-0000000?0\
    0001000100010-001010100120020010-10001001010110001111121-001011010212110211101--221110111-2001010100000000010?010010-010000010\
    1001111110001000001000102012121?0001000101100001110110000101110111300110000001--1001000-?2200101010000100000-11-00011100001000\
    100?000010?0110110101000200200??01001001010010011101102000001111103100100000000-0301000-02210101020000000010-1000000-011011001\
    ?????????????????????????????????????????????0??0???11???100?????021011020110010???1101100???????????????????????????????????0\
    0001001100101000001011102002001110?101001010100001101121-11011101021001021111010030110110020000001000000001101010000-011000??1\
    001110-11010110010101?0120020001100001001000110001111121-201?1001131?000211110111100100-0120010101000000000100010110-011000010\
    ????????????????????1110000200?1101100001010100101100-21-10121101031011021111010210110110011000001000000000100010000-011001010\
    000?000100101000101011102002001110?100001010100?01101121-10111101031?01021111010210110110011000101000000100100010000-011001010\
    00010001101010101110101010110001101101001010100101100-21-1012110103101102011101021?1101101310000010201-1-011000100011011101010\
    101011-??00011001?00000121020000-00001000000000101011000010010000-0--000-00-01-?0-00000-1-20010111000000000100000001110001001-\
    ????????????????????111020020011100000001010100001101?21-10111001131201021111010110110110011000101000000000100010000-011001010\
    ?????????????????????????????????????????0?????????????1-110?????022?01021111010????1????????????????????????????????????????1\
    01000001101010101110101010110001101?01001010101-01100-01-1012100?031011010111010210100100?210000010201-1-010-00000011011011010\
    111010-1?000110?111001010002110110000011?120001-0111100000001002001101100000000-2211000-01210100111201-1-010-1000100-000000000\
    ????????????????????????????????????????????????????10?1-100???0?????01020111010??011011003???0???????????????????????????????\
    00000001001111001110100120020011110001001010110001111121-?010110102121102110001112111011022001010100000000010?010010-01100001?\
    100?0?0??00011011010011100020000-000001000101000111?11?1-110???010310010000000100400000-02301010001010101001000000010111101010\
    101?10-10010110010101?0?20020001110001001000110001111121-101110011?12110211000111?1110110?20010101000000000101010110-011000010;";
    
    mpl_rng_set_seed(1);

    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_set_nnodes(2 * ntax, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
//        mpl_matrixi_set_gap_handle(GAP_MISSING, m);
    mpl_matrix_apply_data(m);
    mpl_init_parsimony(m);
    
    mpl_search s;
    s.treelist = mpl_treelist_new(ntax, 100000, 0);
    s.num_taxa = ntax;
    s.use_ratchet = false;
    s.bbreak_type = MPL_TBR_T;
    
    mpl_bbreak bbk;
    memset(&bbk, 0, sizeof(mpl_bbreak));
    // The number of nodes in the bbreak will be the size of the tree
    bbk.num_nodes = 2 * ntax-1;
    // Now the bbkreak struct can be initialised
    mpl_bbreak_init(&s, &bbk);
    bbk.bbktype = MPL_TBR_T;
    bbk.numreps = 10;
    //    bbk.bbktype = MPL_SPR_T;
    mpl_stepwise_init(MPL_AST_RANDOM, bbk.numtaxa, hold, &bbk.stepwise);
    mpl_do_bbreak(&bbk);
    
    mpl_tree* t  = mpl_new_tree(ntax);
    mpl_topol* top = NULL;
    char* nwkresult = NULL;
    int i = 0;
    
    for (i = 0; i < bbk.treelist->num_trees; ++i) {
        top =  mpl_treelist_get_topol(i, bbk.treelist);
        mpl_tree_read_topol(t, top);

        mpl_tree_write_newick(&nwkresult, t);
        printf("TREE Morphy_%i = [&U] %s [length: %.0f]\n", i+1, nwkresult, top->score);
        free(nwkresult);
    }
    
    if (bbk.shortest != 345.0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (bbk.treelist->num_trees != 31) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    mpl_treelist_delete(&s.treelist);
    
    return failn;
}


int test_mutliple_replicate_heuristic_search_large (void)
{
    theader("Test multiple addition sequence searches with large matrix");
    
    int failn = 0;

    long ntax = Miyashita.ntax;
    long nchar = Miyashita.nchar;
    double testscore = 0.0;
    
    int hold = 1; // Hold up to 3 trees
    
    char* rawmatrix = Miyashita.chardat;
    
    mpl_rng_set_seed(1);

    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_set_nnodes(2 * ntax, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
//        mpl_matrixi_set_gap_handle(GAP_MISSING, m);
    mpl_matrix_apply_data(m);
    mpl_init_parsimony(m);
    
    mpl_search s;
    s.treelist = mpl_treelist_new(ntax, 100000, 0);
    s.num_taxa = ntax;
    s.bbreak_type = MPL_TBR_T;
    
    mpl_bbreak bbk;
    memset(&bbk, 0, sizeof(mpl_bbreak));
    // The number of nodes in the bbreak will be the size of the tree
    bbk.num_nodes = 2 * ntax-1;
    // Now the bbkreak struct can be initialised
    mpl_bbreak_init(&s, &bbk);
    bbk.bbktype = MPL_TBR_T;
    bbk.numreps = 10;
    //    bbk.bbktype = MPL_SPR_T;
    mpl_stepwise_init(MPL_AST_ASIS, bbk.numtaxa, hold, &bbk.stepwise);
    mpl_do_bbreak(&bbk);
    
    mpl_tree* t  = mpl_new_tree(ntax);
    mpl_topol* top = NULL;
    char* nwkresult = NULL;
    int i = 0;
    
    for (i = 0; i < bbk.treelist->num_trees; ++i) {
        top =  mpl_treelist_get_topol(i, bbk.treelist);
        mpl_tree_read_topol(t, top);

        mpl_tree_write_newick(&nwkresult, t);
        printf("TREE Morphy_%i = [&U] %s [length: %.0f]\n", i+1, nwkresult, top->score);
        free(nwkresult);
    }
    
    if (bbk.shortest != 345.0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (bbk.treelist->num_trees != 31) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    mpl_treelist_delete(&s.treelist);
    
    return failn;
}


int test_ratchet_search (void)
{
    theader("Test parsimony ratchet search");
    
    int failn = 0;

    int ntax = Miyashita.ntax;
    int nchar = Miyashita.nchar;
    
    int hold = 10; // Hold up to 3 trees
    
    char* rawmatrix = Miyashita.chardat;

    mpl_matrix* m = mpl_matrix_new();
    mpl_matrix_set_nrows(ntax, m);
    mpl_matrix_set_ncols(nchar, m);
    mpl_matrix_set_nnodes(2 * ntax, m);
    mpl_matrix_attach_rawdata(rawmatrix, m);
    //    mpl_matrix_set_gap_handle(GAP_MISSING, m);
    mpl_matrix_apply_data(m);
    mpl_init_parsimony(m);
    
    mpl_search s;
    s.treelist = mpl_treelist_new(ntax, 100000, 0);
    s.num_taxa = ntax;
    s.bbreak_type = MPL_TBR_T;
    
    mpl_bbreak bbk;
    // The number of nodes in the bbreak will be the size of the tree
    bbk.num_nodes = 2 * ntax-1;
    // Now the bbkreak struct can be initialised
    mpl_bbreak_init(&s, &bbk);
    bbk.bbktype = MPL_TBR_T;
    bbk.numreps = 10;
    bbk.nratchets = 50;
    //    bbk.bbktype = MPL_SPR_T;
    mpl_rng_set_seed(MPL_DEFAULT_RSEED);
    mpl_stepwise_init(MPL_AST_RANDOM, bbk.numtaxa, 1, &bbk.stepwise);
    mpl_do_bbreak(&bbk);
    
    mpl_tree* t  = mpl_new_tree(ntax);
    mpl_topol* top = NULL;
    char* nwkresult = NULL;
    int i = 0;
    
    for (i = 0; i < bbk.treelist->num_trees; ++i) {
        top =  mpl_treelist_get_topol(i, bbk.treelist);
        mpl_tree_read_topol(t, top);
        
        mpl_tree_write_newick(&nwkresult, t);
        printf("TREE Morphy_%i = [&U] %s [length: %.0f]\n", i+1, nwkresult, top->score);
        free(nwkresult);
    }
    
    mpl_matrix_delete(&m);
    mpl_treelist_delete(&s.treelist);
    
    return failn;
}
