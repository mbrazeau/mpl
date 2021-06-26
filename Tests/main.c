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
#include "testbitset.h"
#include "testmplnode.h"
#include "testmpltree.h"
#include "testmplnwkreader.h"
#include "testmpltreelist.h"
#include "testtaxblock.h"
#include "testmpltopol.h"
#include "testmplbbreak.h"
#include "testmplmatrix.h"
#include "testmplparsimony.h"
#include "testmplstepwise.h"
#include "testparsshortcut.h"
#include "testtdraw.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    int fails = 0;
    
    // Test mpl_utils.c
    fails += test_mpl_string_append();
    fails += test_appending_integers();
    fails += test_rand_in_range();
    
    // Test bipartitioning
    fails += test_mpl_bitset_basic();
    fails += test_all_bipartitions_small_tree();
    fails += test_all_nontrivial_bipartitions();
    
    // Test mpl_node.c
    fails += test_node_new_delete();
    fails += test_push_node();
    fails += test_push_to_polynode();
    fails += test_pop_desc();
    fails += test_remove_descendant();
    fails += test_basic_bin_traversal();
    fails += test_node_get_sibling();
    fails += test_swap_parent_child();
    fails += test_binary_node_clip();
    
    // Test mpl_tree.c
    fails += test_tree_assembly_from_topology();
    fails += test_binary_postorder();
    fails += test_worst_case_polytomy();
    fails += test_newick_writing();
    fails += test_tree_rebasing();
    fails += test_tree_rebasing_bigger_tree();
    fails += test_perform_all_rerootings_small();
    fails += test_perform_all_rerootings_large();
    
    // Test mpl_topol.c
    fails += test_rebasing_topology();
    fails += test_rebasing_large_topology();
    fails += test_topology_comparison();
    fails += test_topology_comparison_after_rebase();
    fails += test_negative_topology_comparison();
    fails += test_topology_reuse();
    
    // Test mpl_newick_rdr.c
    fails += test_newick_reader();
    fails += test_newick_reader_bigger_tree();
    fails += test_newick_mult_large_newick_reads();
    fails += test_newick_bulk_reads();
    fails += test_polytomous_postorder();
    
    // Test mpl_taxablock.c
    //    fails += test_taxa_block_basic(argc, argv);
    test_taxa_block_dummylist();
    
    // Test mpl_treelist.c
    fails += test_basic_treelist();
    fails += test_dynamic_treelist();
    fails += test_reject_new();
    fails += test_buffer_clearing();
    fails += test_basic_replication();
    
    // Test mpl_matrix.c
    fails += test_basic_matrix();
    fails += test_skip_closure();
    fails += test_skip_whitespace();
    fails += test_get_char_pointer_from_matrix();
    fails += test_counting_gaps_in_matrix_columns();
    fails += test_count_num_states();
    // TODO: Restore this function:
//    fails += test_parsimony_data_setup();
//    fails += test_matrix_from_io_source(argc, argv);
    
    // Test mpl_tdraw.c
    fails += test_new_tree_drawing();
    
    
    // Test mpl_stepwise.c
    fails += test_initial_fork();
//    fails += test_build_arbitrary_tree();
//    fails += test_build_med_tree();
//    fails += test_build_large_tree();
    
    fails += test_basic_parsimony();
    fails += test_parsimony_on_tree();
    fails += test_fullpass_parsimony();
    fails += test_wagner_fullpass_parsimony();
    fails += test_fullpass_with_large_data_std_parsimony();
    fails += test_fullpass_with_large_data_std_parsimony2();
    fails += test_multiple_small_matrices();
    fails += test_fullpass_with_inapplicables();
    fails += test_fullpass_with_multiple_inapplics();
    
    // Test shortcut parsimony
//    fails += test_parsimony_shortcut();
//    fails += test_parsimony_shortcut_with_rerooting();
//    fails += test_parsimony_shortcut_with_breakout();
//    fails += test_larger_parsimony_shortcut_with_rerooting();
//    fails += test_parsimony_shortcut_multiple_tree();
//    fails += test_parsimony_shortcut_with_wagner();
    fails += test_parsimony_shortcut_with_na_wagner();
//    
//    fails += test_find_char_by_char_mismatches();
    
    // Test mpl_bbreak.c
    //    fails += test_bbreak_reroot_all_for_subtree();
    //    fails += test_bbreak_store_allswaps();
    //    fails += test_bbreak_store_TBR_allswaps();
    fails += test_single_rep_hsearch();
    fails += test_mutliple_replicate_heuristic_search();
//    fails += test_mutliple_replicate_heuristic_search_large();
    fails += test_ratchet_search();

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
