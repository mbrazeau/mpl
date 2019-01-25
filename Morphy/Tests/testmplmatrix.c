//
//  testmplmatrix.c
//  Morphy
//
//  Created by Martin Brazeau on 17/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <string.h>

#include "mpltest.h"
#include "testutils.h"
#include "testmplmatrix.h"
#include "../src/Characters/mpl_chardefs.h"

int test_basic_matrix (void)
{
    theader("Test basic create, initialise, destroy matrix");
    
    int failn = 0;
    
    mpl_matrix* m = NULL;
    
    m = mpl_matrix_new();
    
    if (m == NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_matrix_delete(&m);
    
    return failn;
}

int test_skip_closure (void)
{
    theader("Test skipping closures");
    
    int failn = 0;
    
    char* string = "00010A{0123}BCD0  01FRQ(ACDC)00200111[9981]0091";
    char* s = string;
    
    while (*s) {
        if (strchr(VALID_NEXMAT_PUNC, *s) || strchr(VALID_XREAD_MATPUNC, *s)) {
            if (*s == '[') {
                s = mpl_test_skip_closure(s, '[', ']');
                if (*s != ']') {
                    ++failn;
                    pfail;
                }
                else {
                    ppass;
                }
            }
            else if (*s == '{') {
                s = mpl_test_skip_closure(s, '{', '}');
                if (*s != '}') {
                    ++failn;
                    pfail;
                }
                else {
                    ppass;
                }
            }
            if (*s == '(') {
                s = mpl_test_skip_closure(s, '(', ')');
                if (*s != ')') {
                    ++failn;
                    pfail;
                }
                else {
                    ppass;
                }
            }
        }
        ++s;
    }
    
    return failn;
}

int test_skip_whitespace (void)
{
    theader("Test skipping whitespace");
    
    int failn = 0;
    
    char* string = "00010A{0123} BCD\
    00110   1000001 ;";
    char* s = string;
    
    while (*s) {
        if (strchr(VALID_WS, *s)) {
            s = mpl_test_skip_whitespace(s);
            if (strchr(VALID_WS, *s) && *s != '\0') {
                ++failn;
                pfail;
            }
            else {
                ppass;
            }
            printf("s: %c\n", *s);
        }
        ++s;
    }
    
    return failn;
}

int test_get_char_pointer_from_matrix (void)
{
    theader("Test pointing to cell in char type matrix");
    
    int failn = 0;
    
    long ncols = 15;
    long nrows = 3;
    
    char* string = "i0000 00{012}00 000j0\
                    00000 122[123]2 00(01234)r0\
                    0000k 00000 00000;";
    char* s = NULL;
    
    mpl_matrix* m = NULL;
    m = mpl_matrix_new();
    
    mpl_matrix_set_ncols(ncols, m);
    mpl_matrix_set_nrows(nrows, m);
    mpl_matrix_attach_rawdata(string, m);
    
    s = mpl_test_matrix_get_rawdat_ptr(0, 0, m);
    if (*s != 'i') {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    s = mpl_test_matrix_get_rawdat_ptr(0, 13, m);
    if (*s != 'j') {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    s = mpl_test_matrix_get_rawdat_ptr(0, 20, m);
    if (*s != '1') {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    s = mpl_test_matrix_get_rawdat_ptr(0, 27, m);
    if (*s != '(') {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    s = mpl_test_matrix_get_rawdat_ptr(0, 28, m);
    if (*s != 'r') {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    s = mpl_test_matrix_get_rawdat_ptr(0, 34, m);
    if (*s != 'k') {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    return failn;
}

int test_counting_gaps_in_matrix_columns (void)
{
    theader("Test counting gaps in each matrix column");
    
    int failn = 0;
    
    long ncols = 15;
    long nrows = 7;
    
    char* matrix = "10000 00000 00010\
                    0-000 1-222 00110\
                    0-000 1-222 00110\
                    00000 1-222 0-110\
                    00000 1-022 00110\
                    0-000 1-022 00110\
                    00001 00000 0000-;";
    
    mpl_matrix* m = NULL;
    m = mpl_matrix_new();
    
    mpl_matrix_set_ncols(ncols, m);
    mpl_matrix_set_nrows(nrows, m);
    mpl_matrix_attach_rawdata(matrix, m);
    
    long res = 0;
    
    res = mpl_test_matrix_count_gaps_in_column(0, m);
    if (res != 0) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    res = 0;
    res = mpl_test_matrix_count_gaps_in_column(1, m);
    if (res != 3) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    res = 0;
    res = mpl_test_matrix_count_gaps_in_column(6, m);
    if (res != 5) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    res = 0;
    res = mpl_test_matrix_count_gaps_in_column(11, m);
    if (res != 1) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    res = 0;
    res = mpl_test_matrix_count_gaps_in_column(14, m);
    if (res != 1) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    res = 0;
    
//    mpl_matrix_delete(&m);
    
    return failn;
}

int test_parsimony_data_setup (void)
{
    theader("Test setting up the parsimony types and ranges");
    
    /*
     This is a small test that simply checks whether the program correctly
     partitions the data into two incipient blocks: a parset for standard
     Fitch characters and a second block for Fitch characters with a
     non-trivial set of inapplicable entries.
     */
    
    int failn = 0;
    
    const long ncols = 15;
    const long nrows = 7;
    
    char* matrix =
    //                11111
//   01234      56789 01234
    "10000      00000 00010\
     0-000      1-222 00110\
     0-000      1-222 00110\
     (12)0000   1-222 0-110\
     00000      1-022 00110\
     0-000      1-022 00110\
     00001      00000 0000-;";
    
    mpl_matrix* m = NULL;
    m = mpl_matrix_new();
    
    mpl_matrix_set_ncols(ncols, m);
    mpl_matrix_set_nrows(nrows, m);
    mpl_matrix_attach_rawdata(matrix, m);
//    char* symbols = "0123456789";
//    m->symbols = symbols;
    
    mpl_matrix_apply_data(m);
    
    mpl_test_matrix_setup_parsimony(m);
    
    if (m->parsets[0].start != 0 || m->parsets[0].end != 13) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (m->parsets[1].start != 13 || m->parsets[1].end != 15) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    int i = 0;
    int j = 0;
    
    for (i = 0; i < nrows; ++i) {
        for (j = 0; j < ncols; ++j) {
            printf("%lu", m->cbufs[0].dnset[i][j]);
            if (j < ncols - 1) {
                printf(",\t");
            }
        }
        printf("\n");
    }
    
    unsigned long expected[nrows][ncols] =
//   0  1  2  3  4  5  6  7  8  9  0  1  2  3  4
    {
        {4,  2, 2, 2, 2, 2, 2, 2, 2, 2,          2, 4, 2,          2, 2},
        {2,  2, 2, 2, 4, 8, 8, 8, 2, 2,          4, 4, 2,          1, 1},
        {2,  2, 2, 2, 4, 8, 8, 8, 2, 2,          4, 4, 2,          1, 1},
        {12, 2, 2, 2, 4, 8, 8, 8, 2, 4294967295, 4, 4, 2,          2, 1},
        {2,  2, 2, 2, 4, 2, 8, 8, 2, 2,          4, 4, 2,          2, 1},
        {2,  2, 2, 2, 4, 2, 8, 8, 2, 2,          4, 4, 2,          1, 1},
        {2,  2, 2, 4, 2, 2, 2, 2, 2, 2,          2, 2, 4294967295, 2, 2}
    };
   
    for (i = 0; i < nrows; ++i) {
        for (j = 0; j < ncols; ++j) {
            if (expected[i][j] != m->cbufs[0].dnset[i][j]) {
                ++failn;
                pfail;
                printf("position i: %i, %i\n", i, j);
            }
            else {
                ppass;
            }
        }
    }
    
    mpl_matrix_delete(&m);
    
    return failn;
}
