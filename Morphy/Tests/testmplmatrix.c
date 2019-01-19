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
