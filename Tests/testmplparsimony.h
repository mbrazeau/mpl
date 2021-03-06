//
//  mpltestparsimony.h
//  Morphy
//
//  Created by Martin Brazeau on 28/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpltestparsimony_h
#define mpltestparsimony_h

int test_basic_parsimony (void);
int test_parsimony_on_tree (void);
int test_fullpass_parsimony (void);
int test_wagner_fullpass_parsimony (void);
int test_fullpass_with_large_data_std_parsimony (void);
int test_fullpass_with_large_data_std_parsimony2 (void);
int test_fullpass_with_inapplicables (void);
int test_fullpass_with_multiple_inapplics (void);
int test_multiple_small_matrices (void);
int test_multiple_small_ordered_matrices (void);
int test_wagner_na_parsimony (void);
int test_wagner_na_big_single_char (void);
int test_find_char_by_char_mismatches (void);
#endif /* mpltestparsimony_h */
