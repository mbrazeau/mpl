//
//  testmplbbreak.c
//  Morphy
//
//  Created by Martin Brazeau on 15/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//
#include "mpltest.h"
#include "testutils.h"

#include "testmplbbreak.h"
#include "../src/Trees/mpl_node.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "../src/Trees/mpl_newick_rdr.h"

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
    // The number of nodes in the bbreak will be the size of the tree
    mpl_tree_traverse(t);
    bbk.num_nodes = t->size;
    // Now the bbkreak struct can be initialised
    mpl_bbreak_init(&s, &bbk);
    
    mpl_test_branch_swap(t, &bbk);
    
    int i = 0;
//    for (i = 0; i < bbk.treelist->num_trees; ++i) {
//        mpl_tree_read_topol(t, &bbk.treelist->trees[i]);
//        mpl_tree_write_newick(&nwkout, t);
//        printf("TREE Morphy_%i\t=\t[&U] %s\n", (i + 1), nwkout);
//        free(nwkout);
//    }
    
    printf("\nTBR complete. %li rearrangements tried\n", bbk.num_rearrangs);
    
    return failn;
}
