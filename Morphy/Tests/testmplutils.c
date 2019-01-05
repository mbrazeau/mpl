//
//  testmplutils.c
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "mpltest.h"

#include "mpl_utils.h"
#include "testmplutils.h"

int test_mpl_string_append (void)
{
    theader("Test appending to mpl_str");
    
    int failn = 0;
    
    mpl_str* tstr1 = mpl_str_new(2);
    mpl_str* tstr2 = mpl_str_new(5);
    
    char* str1 = "This is a string";
    char* str2 = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    int i = 0;
    do {
        mpl_str_append(str1[i], tstr1);
        ++i;
    } while (str1[i]);
    
    i = 0;
    do {
        mpl_str_append(str2[i], tstr2);
        ++i;
    } while (str2[i]);
    
    printf("Test string 1: %s\n", mpl_str_c(tstr1));
    printf("Test string 2: %s\n", mpl_str_c(tstr2));
    
    if (strcmp(mpl_str_c(tstr1), str1)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (strcmp(mpl_str_c(tstr2), str2)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (!strcmp(mpl_str_c(tstr2), mpl_str_c(tstr1))) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_str_delete(&tstr1);
    mpl_str_delete(&tstr2);
    
    return failn;
}

int test_appending_integers (void)
{
    theader("Test appending integers to mpl_string");
    
    int failn = 0;
    
    mpl_str* intstr1 = mpl_str_new(1);
    mpl_str* intstr2 = mpl_str_new(1);
    mpl_str* intstr3 = mpl_str_new(1);
    
    long i = 12;
    long j = 288291;
    long k = INT_MAX;
    
    mpl_str_append_int(i, intstr1);
    mpl_str_append_int(j, intstr2);
    mpl_str_append_int(k, intstr3);
    
    // Verify the outputs
    printf("i as string: %s\n", mpl_str_c(intstr1));
    printf("j as string: %s\n", mpl_str_c(intstr2));
    printf("k as string: %s\n", mpl_str_c(intstr3));
    
    if (strcmp(mpl_str_c(intstr1), "12")) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (strcmp(mpl_str_c(intstr2), "288291")) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    char* longmax = NULL;
    if (CHAR_BIT * sizeof(long) == 64) {
        longmax = "9223372036854775807";
    }
    else {
        longmax = "2147483647";
    }
    
    if (!strcmp(mpl_str_c(intstr3), longmax)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    return failn;
}
