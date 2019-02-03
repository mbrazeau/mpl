//
//  testmpltreelist.c
//  Morphy
//
//  Created by Martin Brazeau on 11/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "mpltest.h"
#include "testutils.h"
#include "testmpltreelist.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "../src/Trees/mpl_newick_rdr.h"
#include "../src/Trees/mpl_treelist.h"

int test_basic_treelist (void)
{
    theader("Test basic treelist allocations, setting and getting");
    
    int failn = 0;
    
    long ntax = 10;
    long numtrees = 12;
    char* newicks[] = {
        "((((1,((2,7),(5,9))),(4,8)),6),(3,10));",
        "((1,3),(((((((2,10),6),8),9),7),5),4));",
        "(((1,(((2,4),3),((7,9),8))),(5,10)),6);",
        "(((((((1,8),(3,4)),(2,6)),9),7),5),10);",
        "((((((1,(4,6)),10),5),9),((2,8),7)),3);",
        "((((8,4),(((5,9),(2,7)),1)),6),(10,3));", // Same as 0
        "((((((((1,2),3),(8,10)),9),7),4),6),5);",
        "(((((1,5),(((2,10),3),4)),9),6),(7,8));",
        "((((((1,6),(3,(4,8))),7),10),(2,5)),9);",
        "(((((1,2),10),(3,(5,((7,8),9)))),4),6);",
        "(1,(((2,(((3,6),((5,10),7)),4)),9),8));",
        "((((((1,10),5),((4,6),7)),2),9),(3,8));"
    };
    
    mpl_topol* top = mpl_topol_new(ntax);
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    mpl_treelist* tl = mpl_treelist_new(ntax, numtrees, 0);
    
    int i = 0;
    char* nwkout = NULL;
    
    for (i = 0; i < numtrees; ++i) {
        mpl_newick_read(newicks[i], top, &rdr);
        mpl_tree_read_topol(t, top);
        mpl_tree_write_newick(&nwkout, t);
        printf("%s\n", nwkout);
        free(nwkout);
        nwkout = NULL;
        mpl_tree_checker(t);
        mpl_treelist_add_tree(false, t, tl);
        mpl_topol_reset(ntax, top);
    }
    
    // Delete top so that the pointer can be used to get from the treelist.
    mpl_topol_delete(&top);
    
    printf("\nThe resultant trees from the list:\n");
    for (i = 0; i < numtrees; ++i) {
        top = mpl_treelist_get_topol(i, tl);
        mpl_tree_read_topol(t, top);
        mpl_tree_checker(t);
        mpl_tree_write_newick(&nwkout, t);
        printf("%s\n", nwkout);
        free(nwkout);
        nwkout = NULL;
    }
    
    printf("\nCheck the topologies are equal:\n");
    
    mpl_topol* checktop = mpl_topol_new(ntax);
    
    for (i = 0; i < numtrees; ++i) {
        mpl_topol_reset(ntax, checktop);
        mpl_newick_read(newicks[i], checktop, &rdr);
        mpl_tree_read_topol(t, checktop);
        mpl_tree_record_topol(checktop, t);
        
        top = mpl_treelist_get_topol(i, tl);
        
        if (mpl_topol_compare(top, checktop)) {
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

// Test dynamic extension
int test_dynamic_treelist (void)
{
    theader("Test basic treelist allocations, setting and getting");
    
    int failn = 0;
    
    long ntax = 10;
    long numtrees = 12;
//    long extension_rate = 1;
    char* newicks[] = {
        "((((1,((2,7),(5,9))),(4,8)),6),(3,10));",
        "((1,3),(((((((2,10),6),8),9),7),5),4));",
        "(((1,(((2,4),3),((7,9),8))),(5,10)),6);",
        "(((((((1,8),(3,4)),(2,6)),9),7),5),10);",
        "((((((1,(4,6)),10),5),9),((2,8),7)),3);",
        "((((8,4),(((5,9),(2,7)),1)),6),(10,3));", // Same as 0
        "((((((((1,2),3),(8,10)),9),7),4),6),5);",
        "(((((1,5),(((2,10),3),4)),9),6),(7,8));",
        "((((((1,6),(3,(4,8))),7),10),(2,5)),9);",
        "(((((1,2),10),(3,(5,((7,8),9)))),4),6);",
        "(1,(((2,(((3,6),((5,10),7)),4)),9),8));",
        "((((((1,10),5),((4,6),7)),2),9),(3,8));"
    };
    
    mpl_topol* top = mpl_topol_new(ntax);
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    // NOTE: Here we set less than numtrees to be the maxtrees setting
    mpl_treelist* tl = mpl_treelist_new(ntax, 1, 1);
    
    int i = 0;
    char* nwkout = NULL;
    
    for (i = 0; i < numtrees; ++i) {
        mpl_newick_read(newicks[i], top, &rdr);
        mpl_tree_read_topol(t, top);
        mpl_tree_write_newick(&nwkout, t);
        printf("%s\n", nwkout);
        free(nwkout);
        nwkout = NULL;
        mpl_tree_checker(t);
        mpl_treelist_add_tree(false, t, tl);
        mpl_topol_reset(ntax, top);
    }
    
    // Delete top so that the pointer can be used to get from the treelist.
    mpl_topol_delete(&top);
    
    printf("\nThe resultant trees from the list:\n");
    for (i = 0; i < numtrees; ++i) {
        top = mpl_treelist_get_topol(i, tl);
        mpl_tree_read_topol(t, top);
        mpl_tree_checker(t);
        mpl_tree_write_newick(&nwkout, t);
        printf("%s\n", nwkout);
        free(nwkout);
        nwkout = NULL;
    }
    
    printf("\nCheck the topologies are equal:\n");
    
    mpl_topol* checktop = mpl_topol_new(ntax);
    
    for (i = 0; i < numtrees; ++i) {
        mpl_topol_reset(ntax, checktop);
        mpl_newick_read(newicks[i], checktop, &rdr);
        mpl_tree_read_topol(t, checktop);
        mpl_tree_record_topol(checktop, t);
        
        top = mpl_treelist_get_topol(i, tl);
        
        if (mpl_topol_compare(top, checktop)) {
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

// Test rejecting additional topologies
int test_reject_new (void)
{
    theader("Test rejecting new trees when list is not dynamic");
    
    int failn = 0;
    
    long ntax = 10;
    long numtrees = 12;
    long maxtrees = 5;
    //    long extension_rate = 1;
    char* newicks[] = {
        "((((1,((2,7),(5,9))),(4,8)),6),(3,10));",
        "((1,3),(((((((2,10),6),8),9),7),5),4));",
        "(((1,(((2,4),3),((7,9),8))),(5,10)),6);",
        "(((((((1,8),(3,4)),(2,6)),9),7),5),10);",
        "((((((1,(4,6)),10),5),9),((2,8),7)),3);",
        "((((8,4),(((5,9),(2,7)),1)),6),(10,3));", // Same as 0
        "((((((((1,2),3),(8,10)),9),7),4),6),5);",
        "(((((1,5),(((2,10),3),4)),9),6),(7,8));",
        "((((((1,6),(3,(4,8))),7),10),(2,5)),9);",
        "(((((1,2),10),(3,(5,((7,8),9)))),4),6);",
        "(1,(((2,(((3,6),((5,10),7)),4)),9),8));",
        "((((((1,10),5),((4,6),7)),2),9),(3,8));"
    };
    
    mpl_topol* top = mpl_topol_new(ntax);
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    // NOTE: Here we set less than numtrees to be the maxtrees setting
    mpl_treelist* tl = mpl_treelist_new(ntax, maxtrees, 0);
    
    int i = 0;
    char* nwkout = NULL;
    
    for (i = 0; i < numtrees; ++i) {
        mpl_newick_read(newicks[i], top, &rdr);
        mpl_tree_read_topol(t, top);
        mpl_tree_write_newick(&nwkout, t);
        printf("%s\n", nwkout);
        free(nwkout);
        nwkout = NULL;
        mpl_tree_checker(t);
        mpl_treelist_add_tree(false, t, tl);
        mpl_topol_reset(ntax, top);
    }
    
    // Delete top so that the pointer can be used to get from the treelist.
    if (tl->num_trees != maxtrees) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_delete_tree(&t);
    return failn;
}
// Test clearing buffer
