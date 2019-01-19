//
//  testmplmatrix.h
//  Morphy
//
//  Created by Martin Brazeau on 17/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef testmplmatrix_h
#define testmplmatrix_h

#include "../src/Characters/mpl_matrix.h"

int test_basic_matrix (void);
int test_skip_closure (void);
int test_skip_whitespace (void);
int test_get_char_pointer_from_matrix (void);


char* mpl_test_matrix_get_rawdat_ptr (const long row, const long col, mpl_matrix* m);
char* mpl_test_skip_closure (const char *closure, const char openc, const char closec);
char* mpl_test_skip_whitespace (char* c);

#endif /* testmplmatrix_h */
