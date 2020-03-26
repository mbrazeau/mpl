//
//  testbitset.c
//  morphy_nui_v03
//
//  Created by Martin Brazeau on 24/03/2020.
//
#include "mpltest.h"

#include "testbitset.h"
#include "mpl_bitset.h"
#include "mpl_utils.h"
#include "mpl_tree.h"
#include "mpl_topol.h"
#include "mpl_newick_rdr.h"

void print_bits(mpl_bitset* bits)
{
    int i = 0;
    for (i = bits->nfields-1; i >= 0; --i) {
        int j = 0;
        unsigned long b = (1UL << (NULONGBITS-1));
        for (j = 0; j < NULONGBITS; ++j) {
            
            if (bits->data[i] & (b >> j)) {
                printf("%i", 1);
            } else {
                printf("%i", 0);
            }
        }
    }
}

int test_mpl_bitset_basic (void)
{
    theader("Test basic test of bitset.");
    
    int failn = 0;
    
    mpl_bitset* bits = NULL;
    bits = mpl_bitset_new(100);
    
    mpl_bitset_set(2, bits);
    mpl_bitset_set(80, bits);
    
    mpl_str* splits = mpl_str_new(100);
    mpl_bitset2str(splits, bits);
    
    char* splitstr = mpl_str_c(splits);
//    print_bits(bits);
    printf("%s", splitstr);
    printf("\n");
    
    mpl_str_delete(&splits);
    
    if (bits->data[0] != 0x4) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (bits->data[1] != (0x1 << 16)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    return failn;
}

int test_all_bipartitions_small_tree (void)
{
    theader("Test making all bipartitions for a small tree");
    
    int failn = 0;
    
    const long numtaxa = 5;
    const long numnodes = 2 * numtaxa - 1;
    char* nwkstring = "((1,5),((2,4),3));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    top.newick = NULL;
    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
    mpl_str** biparts = (mpl_str**)safe_calloc(numnodes, sizeof(mpl_str*));
    
    int i = 0;
    for (i = 0; i < numnodes; ++i) {
        biparts[i] = mpl_str_new(numtaxa);
    }
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_tree_traverse(t);
    
    char* bipart = NULL;
    for (i = 0; i < t->num_nodes; ++i) {
        mpl_bitset2str(biparts[i], t->nodes[i].bipart);
        bipart = mpl_str_c(biparts[i]);
        printf("%s", bipart);
        printf("\n");
    }
    
    mpl_delete_tree(&t);
    mpl_topol_cleanup(&top);
    
    return failn;
}

int test_all_nontrivial_bipartitions (void)
{
    theader("Test making all bipartitions for a small tree");
    
    int failn = 0;
    
    const long numtaxa = 10;
//    const long numnodes = 2 * numtaxa - 1;
    char* nwkstring = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    top.newick = NULL;
    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
    mpl_str** biparts = (mpl_str**)safe_calloc(numtaxa-1, sizeof(mpl_str*));
    
    int i = 0;
    for (i = 0; i < numtaxa-1; ++i) {
        biparts[i] = mpl_str_new(numtaxa);
    }
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_tree_traverse(t);
    
    char* bipart = NULL;
    int j = 0;
    for (i = t->num_taxa, j = 0; i < t->num_nodes; ++i, ++j) {
        mpl_bitset2str(biparts[j], t->nodes[i].bipart);
        bipart = mpl_str_c(biparts[j]);
        printf("%s", bipart);
        printf("\n");
    }
    
    mpl_delete_tree(&t);
    mpl_topol_cleanup(&top);
    
    return failn;
}
