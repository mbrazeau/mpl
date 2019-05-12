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
    
typedef void* mpl_handle;

mpl_handle* mpl_handle_new(void);
int         mpl_handle_delete(mpl_handle* handl);
    
// TODO: Use error codes
int         mpl_set_dimensions(const long ntax, const long nchar, mpl_handle handl);
long        mpl_get_ntax(const mpl_handle handl);
long        mpl_get_nchar(const mpl_handle handl);
int         mpl_load_matrix(const mpl_data_t datype, char* matrix, mpl_handle handl);
int         mpl_attach_rawdata(const mpl_data_t datype,const char* rawmatrix, mpl_handle handl);
int         mpl_attach_symbols(const char* symbols, mpl_handle handl);
//          Set character types
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
//          Set hold value
int         mpl_set_hold(const int hold, mpl_handle handl);
//          Get hold value
int         mpl_get_hold(const mpl_handle handl);
//          Set maxtrees
int         mpl_set_maxtrees(const long maxtrees, mpl_handle handle);
//          Get maxtrees
long        mpl_get_maxtrees(const mpl_handle handle);
//          Set autoincrease value
//          Get autoincrease value
//          Set addition sequence type
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
long        mpl_get_num_trees(mpl_handle handl);
char*       mpl_get_newick(const long tnum, mpl_handle handl);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* mpl_h */
