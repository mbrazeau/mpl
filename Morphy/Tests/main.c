//
//  main.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <stdio.h>
#include "testmplutils.h"
#include "mpltest.h"
#include "testmplnode.h"
#include "testmpltree.h"
#include "testmplnwkreader.h"
#include "testtaxblock.h"
#include "testmpltopol.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    int fails = 0;
    
    // Test mpl_utils.c
    fails += test_mpl_string_append();
    fails += test_appending_integers();
    
    // Test mpl_node.c
    fails += test_node_new_delete();
    fails += test_push_node();
    fails += test_push_to_polynode();
    fails += test_pop_desc();
    fails += test_remove_descendant();
    fails += test_basic_bin_traversal();
    fails += test_node_get_sibling();
    fails += test_swap_parent_child();
    
    // Test mpl_tree.c
    fails += test_tree_assembly_from_topology();
    fails += test_binary_postorder();
    fails += test_worst_case_polytomy();
    fails += test_newick_writing();
    fails += test_tree_rebasing();
    fails += test_tree_rebasing_bigger_tree();
    
    // Test mpl_topol.c
    fails += test_rebasing_topology();
    fails += test_rebasing_large_topology();
    fails += test_topology_comparison();
    fails += test_topology_comparison_after_rebase();
    fails += test_negative_topology_comparison();
    
    // Test mpl_newick_rdr.c
    fails += test_newick_reader();
    fails += test_newick_reader_bigger_tree();
    fails += test_newick_mult_large_newick_reads();
    fails += test_polytomous_postorder();
    
    // Test mpl_taxablock.c
    fails += test_taxa_block_basic(argc, argv);
    
    printf("\n\nTest summary:\n\n");
    if (fails) {
        psumf(fails);
    }
    else {
        psump;
    }
    printf("\n");
    
    return fails;
    
}
