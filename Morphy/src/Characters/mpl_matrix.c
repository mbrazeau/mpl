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
#include <stdio.h>

#include "../mpl_defs.h"
#include "../mpl_utils.h"
#include "mpl_matrix.h"
#include "mpl_charbuf.h"
#include "mpl_parsim.h"

static void mpl_set_matrix_defaults(mpl_matrix* m);
static MPL_RETURN mpl_matrix_verify_data(mpl_matrix* m);
static void mpl_matrix_setup_parsimony(mpl_matrix* m);
static void mpl_matrix_write_discr_parsim_to_buffer(mpl_parsdat* pd, mpl_matrix* m);
static void mpl_count_chartypes(mpl_matrix* m);
static void mpl_count_inapplics_by_parstype(mpl_matrix* m);
static char mpl_get_opposite_bracket(const char bracket);
static char* mpl_matrix_get_rawdat_ptr(const long row, const long col, const mpl_matrix* m);
static long mpl_matrix_count_gaps_in_column(const long col, const mpl_matrix* m);
static char* mpl_skip_closure(const char *closure, const char openc, const char closec);
static char* mpl_skip_whitespace(char *c);
static inline mpl_discr mpl_convert_gapsymb(const mpl_gap_t gaptype);
static mpl_discr mpl_char2bitset(const char c, const bool isNAtype, mpl_matrix* m);
static mpl_discr mpl_rawcharptr2bitset(char* cp, const long colnum, mpl_matrix* m);
static void mpl_matrix_get_symbols_from_matrix(mpl_matrix* m);
static void mpl_matrix_setup_cell_ptrs(mpl_matrix* m);
/*******************************************************************************
 *                                                                             *
 *  PUBLIC FUNCTION DEFINITIONS                                                *
 *                                                                             *
 ******************************************************************************/

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
    int i = 0;
    mpl_matrix* mi = NULL;
    mi = *m;
    
    if (mi != NULL) {
        
        safe_free(mi->charinfo);
        safe_free(mi->symbols);
        // NOTE: This assumes the raw data are copied.
        safe_free(mi->rawdata);
        
        for (i = 0; i < mi->ndatypes; ++i) {
            mpl_charbuf_cleanup(&mi->cbufs[i]);
        }
        
        for (i = 0; i < mi->nparsets; ++i) {
            mpl_parsim_cleanup_parsdat(&mi->parsets[i]);
        }
        
        for (i = 0; i < mi->num_rows; ++i) {
            safe_free(mi->rcells[i]);
        }
        safe_free(mi->rcells);
        
        safe_free(mi->parsets);
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
MPL_RETURN mpl_matrix_set_nrows(long num_rows, mpl_matrix* m)
{
    m->num_rows = num_rows;
    
    return MPL_SUCCESS;
}


/**
 Gets the number of rows in the matrix.

 @param m A pointer to an instance of a data matrix.
 @return The number of rows (terminal taxa) in the data matrix.
 */
long mpl_matrix_get_nrows(const mpl_matrix* m)
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

long mpl_matrix_get_ncols(const mpl_matrix* m)
{
    return m->num_cols;
}

MPL_RETURN mpl_matrix_set_nnodes(const long num_nodes, mpl_matrix* m)
{
    m->num_nodes = num_nodes;
    
    return MPL_SUCCESS;
}

long mpl_matrix_get_nnodes(const mpl_matrix* m)
{
    return m->num_nodes;
}

MPL_RETURN  mpl_matrix_init
(const long nrows, const long ncols, const long nnodes, mpl_matrix* m)
{
    long i = 0;
    long j = 0;
    
    MPL_RETURN ret = MPL_SUCCESS;
    
    if ((ret = mpl_matrix_set_nrows(nrows, m))) {
        return ret;
    }
    
    if ((ret = mpl_matrix_set_ncols(ncols, m))) {
        return ret;
    }
    
    if ((ret = mpl_matrix_set_nnodes(nnodes, m))) {
        return ret;
    }
    
    return ret;
}

MPL_RETURN mpl_matrix_attach_rawdata(const char* rawdat, mpl_matrix* m)
{
    RET_IF_NULL(rawdat);
    RET_IF_NULL(m);

    MPL_RETURN ret = MPL_SUCCESS;
    long i = 0;
    long j = 0;
    long matlen = 1;
    char* matcopy = NULL;
    
    if (m->num_cols == 0 || m->num_rows == 0) {
        // There are no dimensions supplied, so the program cannot verify the
        // input matrix
        return MPL_NODIMENSIONS;
    }
    
    if (m->rawdata != NULL) {
        return MPL_ILLEGOVERWRITE;
    }
    
    matlen += strlen(rawdat);
    matcopy = safe_calloc(matlen, sizeof(char));
    strcpy(matcopy, rawdat);
    m->rawdata = matcopy;
    
    // Verify after copying because the verification function needs to use some
    // information in the matrix. This could be changed if some optimisation
    // is needed for larger datasets, but that seems unlikely.
    ret = mpl_matrix_verify_data(m);
    
    if (ret != MPL_SUCCESS) {
        safe_free(m->rawdata);
    }
    
    // Check if symbols are loaded:
    if (m->nsymb == 0) {
        // Do a run through the matrix adding all the symbols
        mpl_matrix_get_symbols_from_matrix(m);
    }
    
    m->rcells = (char***)safe_calloc(m->num_rows, sizeof(char**));
    if (m->rcells == NULL) {
        // TODO: Cleanup matrix and exit;
        return MPL_NOMEM;
    }
    
    for (i = 0; i < m->num_rows; ++i) {
        m->rcells[i] = (char**)safe_calloc(m->num_cols, sizeof(char*));
        if (m->rcells[i] == NULL) {
            for (j = 0; j < i; ++j) {
                safe_free(m->rcells[j]);
            }
            safe_free(m->rcells);
            return MPL_NOMEM;
        }
    }

    // Set all the char pointers to the 'cells'
    mpl_matrix_setup_cell_ptrs(m);

    // TODO: This becomes more important with  other data  types
    mpl_count_chartypes(m);
    // Set up the databuffers needed: discrete, continuous, and "model"-based
    // For each type, set up enough memory to handle the characters
    for (i = 0; i < m->ndatypes; ++i) {
        // TODO: create a return from this and check it
        mpl_charbuf_init((mpl_data_t)i, m->num_rows, m->datypes[i], &m->cbufs[i]);
    }
    
    return ret;
}

MPL_RETURN mpl_matrix_set_parsim_t(const long ind, const mpl_parsim_t ptype, mpl_matrix* m)
{
    if (ind >= m->num_cols) {
        return MPL_OUTOFBOUNDS;
    }
    
    m->charinfo[ind].parsimtype = ptype;
    
    return MPL_SUCCESS;
}

MPL_RETURN mpl_matrix_get_parsim_t(mpl_parsim_t* r, const long ind, mpl_matrix* m)
{
    if (ind >= m->num_cols) {
        return MPL_OUTOFBOUNDS;
    }
    
    *r = m->charinfo[ind].parsimtype;
    
    return MPL_SUCCESS;
}


MPL_RETURN mpl_matrix_set_gap_handle(const mpl_gap_t gaptype, mpl_matrix* m)
{
    // TODO: If this changes the gaptype, you need to trigger a reset of the matrix
    m->gaphandl = gaptype;
    
    return MPL_SUCCESS;
}


MPL_RETURN mpl_matrix_apply_data(mpl_matrix* m)
{
    RET_IF_NULL(m);
    
    int i = 0;
    
    // Everything after here needs to be re-done if character parameters are
    // changed, in particular the parsimony type. This could be moved to its
    // own function and called, for instance, when a parsimony type or weight
    // is changed.
    if (m->optimality == OPTIM_PARSIM) {
        mpl_matrix_setup_parsimony(m);
        mpl_parsim_assign_stateset_ptrs(&m->cbufs[MPL_DISCR_T]);
    }
    else {
        return MPL_NOTIMPLEMENTED;
    }
    
    for (i = 0; i < m->nparsets; ++i) {
        mpl_parsim_setup_tips(m, &m->parsets[i]);
    }
    
    return MPL_SUCCESS;
}

MPL_RETURN mpl_matrix_add_symbol(const char symb, mpl_matrix* m)
{
    MPL_RETURN ret = MPL_SUCCESS;
    
    RET_IF_NULL(m);
    
    if (symb == '\0') {
        return MPL_BADSYMBOL;
    }
    
    if (strchr(VALID_STATESYMB, symb) == NULL) {
        return MPL_BADSYMBOL;
    }
    
    if (strchr(m->symbols, symb) == NULL) {
        m->symbols[m->nsymb] = symb;
        ++m->nsymb;
        m->symbols[m->nsymb] = '\0';
    }

    return ret;
}

MPL_RETURN  mpl_matrix_report(mpl_matrix *m)
{
    MPL_RETURN ret = MPL_SUCCESS;
    
    RET_IF_NULL(m);
    
    printf("Dataset has %li characters\n", m->num_cols);
    printf("\t %li characters are parsimony uninformative\n", -1L);
    
    long nNA = 0;
    
    int i = 0;
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            nNA += (m->parsets[i].end - m->parsets[i].start);
        }
    }
    
    printf("\t %li characters are treated as type \"inapplicable\"\n", nNA);
    
    
    return ret;
}


/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/

static void mpl_set_matrix_defaults(mpl_matrix* m)
{
    m->optimality   = OPTIM_PARSIM;
    m->missingsymb  = DEFAULT_MISSING_SYMB;
    m->gapsymb      = DEFAULT_GAP_SYMB;
    m->unknownsymb  = DEFAULT_UNKNOWN_SYMB;
    m->gaphandl     = GAP_INAPPLIC;
    m->nsymb        = 0;
    
    safe_free(m->symbols);
    
    m->symbols      = (char*)safe_calloc(1 + strlen(VALID_STATESYMB), sizeof(char));
    m->nsymb        = 0;
    //strcpy(m->symbols, VALID_STATESYMB);
}

static MPL_RETURN mpl_matrix_verify_data(mpl_matrix* m)
{
    MPL_RETURN ret = MPL_SUCCESS;
    
    char* c = NULL;
    
    // Check for a terminal semicolon by finding the end of the string
    // (This isn't terribly safe).
    c = m->rawdata;
    while (*c) {
        ++c;
    }
    --c;
    if (*c != ';') {
        return MPL_NOSEMICOLON;
    }
    
    // Check dimensions: should be able to get the terminal semicolon by using
    // the matrix dimensions.
    c = NULL;
    c = mpl_matrix_get_rawdat_ptr(m->num_rows, m->num_cols, m);
    if (*c != ';') {
        return MPL_BADDIMENSIONS;
    }

    // Check symbols are valid
    c = m->rawdata;
    while (*c != ';') {
        if (!strchr(VALIDSYMB, *c)) {
            printf("ERROR: symbol %c not recognized.\n", *c);
            return MPL_BADSYMBOL;
        }
        ++c;
    }
    
    // TODO: Finish verification checks. These aren't vital, as external
    // libraries will be used to handle file i/o, but Morphy's state
    // conversion stuff is idiosyncratic enough that extra checks are worthwhile
    // Check all open parentheses have a close
    // Check no multistates with inapplicable
    
    return ret;
}

static void mpl_matrix_delete_parsets(mpl_matrix* m)
{
    int i = 0;

    if (m->parsets != NULL) {

        for (i = 0; i < m->nparsets; ++i) {
            mpl_parsim_cleanup_parsdat(&m->parsets[i]);
        }

        free(m->parsets);
        m->parsets = NULL;
    }
}

static void mpl_matrix_setup_parsimony(mpl_matrix* m)
{
    int i = 0;
    int j = 0;
    long numstd = 0;
    long numna = 0;
    int joint_pars_types = 0;
    long current_range = 0;
    
    // Each parsimony type will have a certain number of columns with inapplicable
    // values. This then can be used to determine the number of columns in the
    // inapplicable parsimony type
//    mpl_count_inapplics_by_parstype(m);
    
    if (m->gaphandl == GAP_INAPPLIC) {
        // Determine each column's inapplicable data count and assign whether
        // the column should be counted as having non-trivial NAs
        mpl_count_inapplics_by_parstype(m);
    }
    
    // This determines the ranges for std type characters and inapplicable-type
    // characters in the main buffer.
    for (i = 0; i < MPL_PARSIM_T_MAX; ++i) {
        
        if (m->parstypes[i] > 0) {
            ++joint_pars_types;
        }
        if (m->nasbytype[i] > 0) {
            if ((m->parstypes[i] - m->nasbytype[i]) > 0) {
                ++joint_pars_types;
            }
        }
        
        numstd += m->parstypes[i] - m->nasbytype[i];
        assert(numstd >= 0);
        numna += m->nasbytype[i];
    }
    
    // Clear any prior parsimony sets:
    mpl_matrix_delete_parsets(m);
    // Set up the parsimony sets
    m->nparsets = joint_pars_types;
    m->parsets = (mpl_parsdat*)safe_calloc(m->nparsets, sizeof(mpl_parsdat));
    // TODO: CHECK RETURN MORE SAFELY?
    assert(m->parsets != NULL);
    
    // Set and size the standard parsimony sets
    for (i = 0; i < m->nparsimtypes; ++i) {
        // Check how many standard characters in this type
        if ((m->parstypes[i] - m->nasbytype[i]) > 0) {
            
            mpl_parsim_set_type(GAP_MISSING, (mpl_parsim_t)i, &m->parsets[j]);
            
            // The range for this type is the current range index + size of the
            // type block
            mpl_parsim_init_parsdat
            (current_range, current_range + (m->parstypes[i] - m->nasbytype[i]), &m->parsets[j]);
            
            
            
            // Update current range
            current_range += (m->parstypes[i] - m->nasbytype[i]);
            ++j;
        }
    }
    
    
    // Set and size the inapplic parsimony sets
    for (i = 0; i < m->nparsimtypes; ++i) {
        // Check how many standard characters in this type
        if (m->nasbytype[i] > 0) {
            
            mpl_parsim_set_type(GAP_INAPPLIC, (mpl_parsim_t)i, &m->parsets[j]);
            
            // The range for this type is the current range index + size of the
            // type block
            mpl_parsim_init_parsdat
            (current_range, current_range + m->nasbytype[i], &m->parsets[j]);
            
            // Set up the nodal index storage
            mpl_parsim_setup_nodal_index_buffers(2 * m->num_rows, &m->parsets[j]);
            
            // Update current range
            current_range += m->nasbytype[i];
            ++j;
        }
    }
    
    // Write the data into the buffers as appropriate
    for (i = 0; i < m->nparsets; ++i) {
        // Write into buffer
        mpl_matrix_write_discr_parsim_to_buffer(&m->parsets[i], m);
    }
}

static void mpl_matrix_convert_into_discrbuffer
(mpl_discr* di, const long coln, mpl_matrix* m)
{
    long i = 0;
    char* cp = NULL;
    
    for (i = 0; i < m->num_rows; ++i) {
        cp = m->rcells[i][coln];//mpl_matrix_get_rawdat_ptr(i, coln, m);
        di[i] = mpl_rawcharptr2bitset(cp, coln, m);
    }
}

static void mpl_matrix_write_discr_parsim_to_buffer
(mpl_parsdat* pd, mpl_matrix* m)
{
    long i = 0;
    
    mpl_discr* colbuf = NULL;
    colbuf = (mpl_discr*)safe_calloc(m->num_rows, sizeof(mpl_discr));
    
    // Loop over all characters in the matrix and check that the character's
    // description matches this parsimony data type. Check if it is the
    // same optimality type (e.g. Fitch, Wagner...) and if it has the same
    // method of treating gaps.
    for (i = 0; i < m->num_cols; ++i) {

        // If column is the same parsimony type as the pardat struct:
        if (m->charinfo[i].parsimtype == pd->parstype) {
            // And if they have the same value for treating gaps:
            if (pd->isNAtype == true && m->charinfo[i].num_gaps > 2) {
                mpl_matrix_convert_into_discrbuffer(colbuf, i, m);
                mpl_parsim_add_data_column_to_buffer
                (colbuf, &m->charinfo[i], &m->cbufs[MPL_DISCR_T], pd);
            }
            else if (pd->isNAtype == false && m->charinfo[i].num_gaps < 3){
                mpl_matrix_convert_into_discrbuffer(colbuf, i, m);
                mpl_parsim_add_data_column_to_buffer
                (colbuf, &m->charinfo[i], &m->cbufs[MPL_DISCR_T], pd);
            }
        }
    }
    
    free(colbuf);
}

/**
 For each character info struct in the charinfo block of the matrix, a count is
 accumulated for each time a particular data type is visited.
 
 @param m A pointer to an instance of a matrix struct.
 */
static void mpl_count_chartypes(mpl_matrix* m)
{
    long i = 0;
    
    // Reset the buffers before counting
    memset(m->datypes, 0, MPL_DATA_T_MAX * sizeof(int));
    memset(m->parstypes, 0, MPL_PARSIM_T_MAX * sizeof(int));
    
    for (i = 0; i < m->num_cols; ++i) {
        // datatype and parsimtype are enumerated types, the datypes array and
        // parstypes arrays in the matrix struct can store a count at each
        // position corresponding to the datatype of the charinfo cell.
        ++m->datypes[m->charinfo[i].datatype];
        // Count the number of data types
        if (m->datypes[m->charinfo[i].datatype] == 1) {
            ++m->ndatypes;
        }
        ++m->parstypes[m->charinfo[i].parsimtype];
        if (m->parstypes[m->charinfo[i].datatype] == 1) {
            ++m->nparsimtypes;
        }
    }
}

static void mpl_count_inapplics_by_parstype(mpl_matrix* m)
{
    long i = 0;
    
    memset(m->nasbytype, 0, MPL_PARSIM_T_MAX * sizeof(int));
    
    
    for (i = 0; i < m->num_cols; ++i) {
        
        m->charinfo[i].num_gaps = mpl_matrix_count_gaps_in_column((const long)i, (const mpl_matrix*)m);
        
        if (m->charinfo[i].num_gaps > 2) {
            ++m->nasbytype[m->charinfo[i].datatype];
        }
    }
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

char* mpl_matrix_get_rawdat_ptr(const long row, const long col, const mpl_matrix* m)
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

static void mpl_matrix_setup_cell_ptrs(mpl_matrix* m)
{
    int i = 0;
    int j = 0;
    char* c = m->rawdata;
    
    c = mpl_skip_whitespace(c);
    
    while (*c != ';' && *c != '\0') {
        
        if (strchr(VALID_WS, *c) == NULL) {
            
            if (strchr(VALID_MPL_MATPUNC, *c) == NULL) {
                // The character is some symbol or other
                m->rcells[i][j] = c;
                ++j;
            } else if (*c != ';') {
                m->rcells[i][j] = c;
                c = mpl_skip_closure(c, *c, mpl_get_opposite_bracket(*c));
                ++j;
            }
        }
        
        if (j >= m->num_cols) {
            ++i;
            j = 0;
        }
        
        ++c;
    }
    
}

static long mpl_matrix_count_gaps_in_column(const long col, const mpl_matrix* m)
{
    long ngaps = 0;
    
    long i = 0;
    char c = 0;
    
    for (i = 0; i < m->num_rows; ++i) {
        c = 0;
        c = *m->rcells[i][col];//*mpl_matrix_get_rawdat_ptr(i, col, m);
        if (c == m->gapsymb) {
            ++ngaps;
        }
    }
    
    return ngaps;
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

static inline mpl_discr mpl_convert_gapsymb(const mpl_gap_t gaptype)
{
    if (gaptype == GAP_MISSING) {
        return MISSING;
    }
    
    return NA;
}

static mpl_discr mpl_char2bitset
(const char c, const bool isNAtype, mpl_matrix* m)
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
//            assert(i >= m->nsymb);
            
            return (mpl_discr)1 << (i + GAPPUSH);
        }
        else if (c == m->missingsymb) {
            return MISSING;
        }
        else if (c == m->gapsymb) {
            if (isNAtype) {
                return mpl_convert_gapsymb(GAP_INAPPLIC);
            }
            else {
                if (m->gaphandl == GAP_INAPPLIC) {
                    // This means it's a trivial gap for the Brazeau,
                    // Guillerme, Smith algorithm, so treat as missing
                    return mpl_convert_gapsymb(GAP_MISSING);
                } else {
                    return mpl_convert_gapsymb(m->gaphandl);
                }
            }
        }
        else if (c == m->unknownsymb) {
            return UNKNOWN;
        }
        
    }
    
    return 0;
}

static mpl_discr mpl_rawcharptr2bitset
(char* cp, const long colnum, mpl_matrix* m)
{
    mpl_discr res = 0;
    bool isNAtype = false;
    
    if (m->gaphandl == GAP_INAPPLIC && m->charinfo[colnum].num_gaps > 2) {
        isNAtype = true;
    }
    
    if (strchr(VALID_MPL_MATPUNC, *cp)) {
        
        char cl = mpl_get_opposite_bracket(*cp);
        while (*cp != cl) {
            ++cp;
            res |= mpl_char2bitset(*cp, isNAtype, m);
        }
    }
    else {
        res = mpl_char2bitset(*cp, isNAtype, m);
    }
    
    return res;
}

static void mpl_matrix_get_symbols_from_matrix(mpl_matrix* m)
{
    char *c = m->rawdata;
    
    while (*c != ';') {
        if (strchr(VALID_STATESYMB, *c)) {
            mpl_matrix_add_symbol(*c, m);
        }
        ++c;
    }
    
    qsort(m->symbols, m->nsymb, sizeof(char), mpl_compchar);
}

/*
 *  TEST INTERFACE FUNCTION DEFINITIONS
 */

//#ifdef DEBUG

char*
mpl_test_matrix_get_rawdat_ptr(const long row, const long col, mpl_matrix* m)
{
    return mpl_matrix_get_rawdat_ptr(row, col, m);
}

void mpl_test_matrix_setup_parsimony(mpl_matrix* m)
{
    mpl_matrix_setup_parsimony(m);
}

long mpl_test_matrix_count_gaps_in_column(const long col, const mpl_matrix* m)
{
    return mpl_matrix_count_gaps_in_column(col, m);
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

//#endif
