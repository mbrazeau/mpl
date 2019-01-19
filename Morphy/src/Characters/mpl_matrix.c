//
//  mpl_matrix.c
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "../mpl_utils.h"
#include "mpl_matrix.h"


static void mpl_set_matrix_defaults(mpl_matrix* m);
static char mpl_get_opposite_bracket(const char bracket);
static char* mpl_skip_closure(const char *closure, const char openc, const char closec);
static char* mpl_skip_whitespace(char *c);
static inline mpl_discr mpl_convert_gapsymb(mpl_matrix* m);
static mpl_discr mpl_char2bitset(const char c, mpl_matrix* m);



/*
 *  PUBLIC FUNCTION DEFINITIONS
 */


/**
 Allocate a new instance of a data matrix. Typically, the program will require
 only one such instance, as it would be unusual to load more than one dataset.

 @return A pointer to an instance of the matrix.
 */
mpl_matrix* mpl_matrix_new(void)
{
    mpl_matrix* ret = NULL;
    
    ret = (mpl_matrix*)safe_calloc(1, sizeof(mpl_matrix));
    
    if (ret != NULL) {
        mpl_set_matrix_defaults(ret);
    }
    
    return ret;
}


/**
 Deletes the matrix object.

 @param m Address of a pointer to the matrix.
 */
void mpl_matrix_delete(mpl_matrix** m)
{
    mpl_matrix* mi = NULL;
    mi = *m;
    
    if (mi != NULL) {
        
        if (mi->charinfo != NULL) {
            free(mi->charinfo);
            mi->charinfo = NULL;
        }
        
        if (mi->symbols != NULL) {
            free(mi->symbols);
            mi->symbols = NULL;
        }
        
        // NOTE: This assumes the raw data are copied.
        if (mi->rawdata != NULL) {
            free(mi->rawdata);
            mi->rawdata = NULL;
        }
        
        free(mi);
    }
    
    *m = NULL;
}


/**
 Set the number of rows that the data matrix will have. This correspondes to the
 number of terminal taxa in the dataset.

 @param num_rows The number of rows (terminal taxa) of the dataset.
 @param m A pointer to an instance of a matrix.
 */
void mpl_matrix_set_nrows(long num_rows, mpl_matrix* m)
{
    m->num_rows = num_rows;
}


/**
 Gets the number of rows in the matrix.

 @param m A pointer to an instance of a data matrix.
 @return The number of rows (terminal taxa) in the data matrix.
 */
long mpl_matrix_get_nrows(mpl_matrix* m)
{
    return m->num_rows;
}


/**
 Sets the number of columns in the data matrix and then allocates memory for
 storage of information about each character. If a prior, non-zero number of
 columns has been specified for the matrix, the function will reset all the
 character info. Thus, if character types and weights etc. have been set, this
 function will re-set those values to defaults/zero. This function should return
 feedback to external callers, as it directly interacts with user input.
 Therefore, it returns an MPL error code.

 @param num_cols The number of columns (characters) the dataset will have
 @param m A pointer to an instance of a matrix.
 @return An MPL error code.
 */
MPL_RETURN mpl_matrix_set_ncols(long num_cols, mpl_matrix* m)
{
    int i = 0;
    
    if (num_cols == 0) {
        return MPL_BADPARAM;
    }
    
    m->num_cols = num_cols;
    
    if (m->charinfo != NULL) {
        free(m->charinfo);
        m->charinfo = NULL;
    }
    
    m->charinfo = (mpl_charinfo*)safe_calloc(m->num_cols, sizeof(mpl_charinfo));
    if (m->charinfo == NULL) {
        return MPL_NOMEM;
    }
    
    // Setup the default values
    for (i = 0; i < m->num_cols; ++i) {
        mpl_charinfo_set_defaults(&m->charinfo[i]);
    }
    
    return MPL_SUCCESS;
}

long mpl_matrix_get_ncols(mpl_matrix* m)
{
    return m->num_cols;
}

MPL_RETURN mpl_matrix_attach_rawdata(char* rawdat, mpl_matrix* m)
{
    if (m->num_cols == 0 || m->num_rows == 0) {
        // There are no dimensions supplied, so the program cannot verify the
        // input matrix
        return MPL_NODIMENSIONS;
    }
    
    if (m->rawdata != NULL) {
        return MPL_ILLEGOVERWRITE;
    }
    
    // Allocate a string to copy the raw data into?
    m->rawdata = rawdat;
    
    return MPL_SUCCESS;
}

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

static void mpl_set_matrix_defaults(mpl_matrix* m)
{
    m->missingsymb  = DEFAULT_MISSING_SYMB;
    m->gapsymb      = DEFAULT_GAP_SYMB;
    m->unknownsymb  = DEFAULT_UNKNOWN_SYMB;
    m->gaphandl     = GAP_INAPPLIC;
}

static char mpl_get_opposite_bracket(const char bracket)
{
    char r = 0;
    
    switch (bracket) {
        case '(' :
            r = ')';
            break;
        case '{' :
            r = '}';
            break;
        case '[' :
            r = ']';
            break;
        case ')' :
            r = '(';
            break;
        case '}' :
            r = '{';
            break;
        case ']' :
            r = '[';
            break;
        default:
            break;
    }
    
    return r;
}

char* mpl_matrix_get_rawdat_ptr(const long row, const long col, mpl_matrix* m)
{
    long  i = 0;
    long  tgt = row * m->num_cols + col;
    char* ret = NULL;
    
    ret = m->rawdata;
    
    // Skip any leading whitespace
    ret = mpl_skip_whitespace(ret);
    
    while(*ret && strchr(VALIDSYMB, *ret)) {
        
        if (!strchr(VALID_WS, *ret)) {
            
            if (!strchr(VALID_MPL_MATPUNC, *ret)) {
                if (i == tgt) {
                    return ret;
                }
                ++i;
            }
            else if (*ret != ';') {
                // If this is the stopping point, return the result
                if (i == tgt) {
                    return ret;
                }
                ret = mpl_skip_closure(ret, *ret, mpl_get_opposite_bracket(*ret));
                ++i;
            }
            else {
                return ret;
            }
        }
        ++ret;
    }
    
    return ret;
}

static char* mpl_skip_closure
(const char *closure, const char openc, const char closec)
{
    if (*closure != openc) {
        return (char*)MPL_BADPARAM;
    }
    char *ret = (char*)closure;
    
    do {
        ++ret;
        if (*ret == closec) {
            return ret;
        }
    } while (*ret);
    
    return NULL;
}

static char* mpl_skip_whitespace(char *c)
{
    while (strchr(VALID_WS, *c) && *c != '\0') {
        ++c;
    }
    
    return c;
}

static inline mpl_discr mpl_convert_gapsymb(mpl_matrix* m)
{
    if (m->gaphandl == GAP_MISSING) {
        return MISSING;
    }
    
    return NA;
}

static mpl_discr mpl_char2bitset(const char c, mpl_matrix* m)
{
    int i = 0;
    
    
    if (c != '\0') {
        
        if (strchr(VALID_STATESYMB, c)) {
            
            // For any of the 'normal' state symbols use the symbol list to
            // create a shift value.
            while (c != m->symbols[i]) {
                ++i;
            }
            
            assert(m->symbols[i] != '\0');
            assert(i >= m->nsymb);
            
            return (mpl_discr)1 << (i + GAPPUSH);
        }
        else if (c == m->missingsymb) {
            return MISSING;
        }
        else if (c == m->gapsymb) {
            return mpl_convert_gapsymb(m);
        }
        else if (c == m->unknownsymb) {
            return UNKNOWN;
        }
        
    }
    
    return 0;
}

/*
 *  TEST INTERFACE FUNCTION DEFINITIONS
 */

#ifdef DEBUG

char*
mpl_test_matrix_get_rawdat_ptr(const long row, const long col, mpl_matrix* m)
{
    return mpl_matrix_get_rawdat_ptr(row, col, m);
}

char* mpl_test_skip_closure
(const char *closure, const char openc, const char closec)
{
    return mpl_skip_closure(closure, openc, closec);
}

char* mpl_test_skip_whitespace(char* c)
{
    return mpl_skip_whitespace(c);
}

#endif
