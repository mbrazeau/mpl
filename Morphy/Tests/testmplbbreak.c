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
