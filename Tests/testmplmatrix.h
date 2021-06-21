//
//  testmplmatrix.h
//  Morphy
//
//  Created by Martin Brazeau on 17/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef testmplmatrix_h
#define testmplmatrix_h

#include "mpl_matrix.h"

int test_basic_matrix (void);
int test_skip_closure (void);
int test_skip_whitespace (void);
int test_get_char_pointer_from_matrix (void);
int test_counting_gaps_in_matrix_columns (void);
int test_parsimony_data_setup (void);
int test_matrix_from_io_source (int argc, const char * argv[]);

char* mpl_test_matrix_get_rawdat_ptr (const long row, const long col, mpl_matrix* m);
long mpl_test_matrix_count_gaps_in_column(const long col, const mpl_matrix* m);
void mpl_test_matrix_setup_parsimony(mpl_matrix* m);
char* mpl_test_skip_closure (const char *closure, const char openc, const char closec);
char* mpl_test_skip_whitespace (char* c);
int test_count_num_states (void);

#endif /* testmplmatrix_h */
