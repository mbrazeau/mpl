//
//  testmplcontree.c
//  morphy_nui_v03
//
//  Created by Martin Brazeau on 19/08/2021.
//

#include "mpltest.h"
#include "testutils.h"
#include "testmpltreelist.h"
#include "mpl_tree.h"
#include "mpl_topol.h"
#include "mpl_newick_rdr.h"
#include "mpl_treelist.h"
#include "mpl_contree.h"
#include "mpl_bipartlist.h"
#include "mpl_bitset.h"
#include "mpl_tdraw.h"

void print_bits(mpl_bitset* bits);

int test_strict_consensus_tree (void)
{
    theader("Test strict consensus");
    
    int i = 0;
    int failn = 0;
    
    long ntax = 10;
    long numtrees = 6;
    
    char* taxa[] = {
        "Alpha",
        "Beta",
        "Gamma",
        "Delta",
        "Epsilon",
        "Eta",
        "Theta",
        "Omega",
        "Rho",
        "Zeta"
    };
    
    mpl_taxablock *tb = mpl_taxablock_new(ntax);
    for (i = 0; i < ntax; ++i) {
        mpl_taxablock_add_taxon(taxa[i], tb);
    }
    
//    long extension_rate = 1;
    char* newicks[] = {
        "((((1,((2,7),(5,9))),(4,8)),6),(3,10));",
        "((((1,((2,7),(5,9))),(4,8)),6),(3,10));",
        "((((1,((2,9),(5,7))),(4,8)),6),(3,10));",
        "((((4,((2,9),(5,7))),(1,8)),6),(3,10));",
        "((((4,((2,9),(5,7))),(1,8)),6),(3,10));",
        "((((8,4),(((5,9),(2,7)),1)),10),(6,3));"
    };
    
    mpl_topol* top = mpl_topol_new(ntax);
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    // NOTE: Here we set less than numtrees to be the maxtrees setting
    mpl_treelist* tl = mpl_treelist_new(ntax, 1, 1);
    
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
    
    // Setup the consensus tree
    mpl_contree *ctree;
    
    ctree = mpl_contree_new(ntax, tl);
    mpl_contree_strict(ctree);
    
//    for (i = 0; i < ctree->biparts->num_splits; ++i) {
//        print_bits(ctree->biparts->biparts[i]);
//        printf("\n");
//    }
    
    printf("\n");
    mpl_tree_write_newick(&nwkout, ctree->t);
    printf("%s\n", nwkout);
    free(nwkout);
    
    mpl_tdraw* tdrw = mpl_tdraw_new(tb);
    
    ctree->t->num_polys = 1;
    mpl_tdraw_do(ctree->t, tdrw);
    
    for (i = 0; i < tdrw->height; ++i) {
        printf("%s", tdrw->canvas[i]);
    }
    
    mpl_tdraw_delete(&tdrw);
    
    // TODO: Delete contree memory
    mpl_delete_tree(&t);
    
    return failn;
}
