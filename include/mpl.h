//
//  mpl.h
//  Morphy
//
//  Created by Martin Brazeau on 19/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_h
#define mpl_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "../src/mpl_defs.h"
        
// TODO: Use error codes throughout file?

typedef void* mpl_handle;

/**
 @brief Creates a new instance of the mpl object.
 
 @discussion A client program can generate as many of these as it needs.
 
 @return A pointer to the new handle object if success. Returns NULL if failed.
 */
mpl_handle* mpl_handle_new(void);

/**
 @brief Deletes the instance of the mpl object, releasing all allocated memory
 for any characters, taxa, trees or any other data.
 
 @param handl A pointer to an instance of the mpl object.
 @return An integer corresponding to an error code. 0 if success;
 negative value for an error.
 */
int         mpl_handle_delete(mpl_handle* handl);

/**
 @brief Sets the dimensions of the input dataset.
 
 @param ntax Sets the number of taxa (rows) in the dataset.
 @param nchar Sets the number of characters (columns) in the dataset.
 @param handl A reference to the mpl object.
 @return An integer corresponding to an error code. 0 if success;
 negative value for an error.
 */
int         mpl_set_dimensions(const long ntax, const long nchar, mpl_handle handl);

/**
 @brief Gets the number of taxa (rows) of the input dataset.
 
 @param handl an instance of the mpl object.
 @return A positive or 0 value if success; negative value for an error.
 */
long        mpl_get_ntax(const mpl_handle handl);

/**
@brief Gets the number of characters (columns) of the input dataset.

@param handl an instance of the mpl object.
@return A positive or 0 value if success; negative value for an error.
*/
long        mpl_get_nchar(const mpl_handle handl);

/**
 @brief Attach a raw character data matrix.
 
 @discussion Attach raw data as a C-style, null-terminated string with a
 terminal semicolon. Whitespace is allowed and will be ignored by the program.
 See program documentation for valid symbols.
 
 @param datype Enumerted type indicating the type of input data (i.e. discrete,
 continuous etc.)
 @param rawmatrix C-style, null-terminated string with terminal semicolon
 consisting of the complete dataset.
 @param handl An instance of the mpl object.
 @return 0 if success; negative value for an error.
 */
int         mpl_attach_rawdata(const mpl_data_t datype, const char* rawmatrix, mpl_handle handl);

/**
@brief Attach a caller-specified list of symbols.

@discussion Allows the caller to specify a list of symbols in the data matrix,
otherwise, the symbols list used by Morphy will be extracted from the matrix.
The symbols list must match the symbols provided in the matrix. When Morphy
extracts symbols from the matrix, their ordering is alphanumeric, according to
their ASCII codes (i.e. "+0123...ABCD...abcd..."). Loading a user-specified
symbols list will override this ordering, like in PAUP. Symbols loaded in either
the list or the matrix must be valid Morphy character state symbols as defined
in the mpl_chardefs.h header file.

@param symbols A C-style (i.e. null-terminated) string of valid state symbols.
@param handl An instance of the mpl object.
@return 0 if success; negative value for an error.
*/
int         mpl_attach_symbols(const char* symbols, mpl_handle handl);


/**
 @brief Set the parsimony type for the dataset.
 
 @discussion Set the parsimony type as either unordered (default) or ordered.
 
 @param index 0-initial index of the character whose parsimony type is to be set.
 @param ptype The parsimony type as defined in mpl_defs.h header file.
 @return 0 if success; negative value for an error.
 */
int mpl_set_parsim_t(const long index, const mpl_parsim_t ptype, mpl_handle handl);

/*!

 @brief Tells Morphy how to treat the gap symbol.

 @discussion The caller can specify the type of gap handling to use before the
 tipdata are applied. The options are documented in the morphydefs.h header file
 but include at least GAP_INAPPLIC, GAP_MISSING, and GAP_NEWSTATE to specify
 inapplicable, missing, or new state values respectively. These values are
 applied to all characters for which they are appropriate.
 
 @param gaphandl The type of gap treatment to be applied (documented in
 mpl_defs.h).
 @param handl An instance of the mpl object.
 @return 0 if success; negative value for an error.
*/
int         mpl_set_gap_handl(const mpl_gap_t gaphandl, mpl_handle handl);
//          Exclude character
//          Include character
//          Add taxon label
//          Delete taxon label
//          Exclude taxon
//          Include taxon

//          Set character weight
int         mpl_set_char_weight(const float weight, const long index, mpl_handle handl);

//          Get character weight
float       mpl_get_char_weight(const long index, mpl_handle handl);

//          Set equal weights
int         mpl_set_equal_weights(mpl_handle handl);

//          Set character parsimony type
//          Get character parsimony type
//          Apply character settings

/**
 @brief Set the number of trees to hold at each step during stepwise addition.
 
 @param hold an integer indicating the number of trees to hold at each step.
 @param handl A constant pointer to an instanec of the mpl object.
 @return 0 if success; negative value for an error.
 */
int         mpl_set_hold(const int hold, mpl_handle handl);

/**
 @brief Returns the number of trees to be held at each step during stepwise
 addition.
 
 @param handl A constant pointer to an instanec of the mpl object.
 @return An integer indicating the hold number.
 */
int         mpl_get_hold(const mpl_handle handl);

/**
 @brief Sets the maximum number of trees to hold in memory.
 
 @discussion This is the maximum number of trees held in memory. Any additional
 trees will be discarded.
 
 @param maxtrees An integer indicating the maximum number of trees to hold in memory.
 @param handl A pointer to an instanec of the mpl object.
 @return 0 if success; negative value for an error.
 */
int         mpl_set_maxtrees(const long maxtrees, mpl_handle handl);

/**
 @brief Returns the maximum number of trees that can be held in memory.
 
 @param handl A constant pointer to an instanec of the mpl object.
 @return An integer indicating the maximum number of trees.
 */
long        mpl_get_maxtrees(const mpl_handle handl);

//          Set autoincrease value
//          Get autoincrease value

/**
 @brief Sets the type of addition sequence to use in obtaining starting trees.
 
 @discussion Three parameters are possible for this function (passed as enumerated
 data types defined in mpl_defs.h:
 
 Random (MPL_AST_RANDOM): Randomises input order of taxa.
 As-is (MPL_AST_ASIS): Uses the input order of taxa.
 None (MPL_AST_INMEM): Skips the addition sequence and uses trees in memory.
 
 @param as A constant value indicating the addition sequence type (as defined in mpl_defs.h
 @param handl A pointer to an instanec of the mpl object.
 @return 0 if success; negative value for an error.
 */
int         mpl_set_addseq(const mpl_addseq_t as, mpl_handle handle);
//          Get addition sequence type
//          Set branch-swapping type
int         mpl_set_brswap(const mpl_bbreak_t bbkt, mpl_handle handle);
//          Get branch-swapping type
//          Set keep number (number of trees to keep per replicate)
//          Get keep number (number of trees to keep per replicate)
int         mpl_set_numreps(const unsigned long nreps, mpl_handle handle);
//          Do search according to parameters
int         mpl_use_ratchet(const bool useratchet, mpl_handle handl);
int         mpl_do_search(mpl_handle handle);
int         mpl_score_tree(double* score, const long index, mpl_handle handl);
long        mpl_get_num_trees(mpl_handle handl);
int         mpl_clear_trees(mpl_handle handl);
int         mpl_add_newick(const char* newick, mpl_handle handl);
char*       mpl_get_newick(const long tnum, mpl_handle handl);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* mpl_h */
