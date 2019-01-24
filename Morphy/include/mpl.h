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
    
// TODO: Need error code defs
// TODO: Need enums that are exposed to client program

typedef void* mpl_handle;

mpl_handle* mpl_handle_new(void);
void        mpl_handle_delete(mpl_handle** handl);
    
// TODO: Use error codes
long        mpl_get_ntax(mpl_handle handl);
int         mpl_set_ntax(long ntax, mpl_handle handl);
int         mpl_get_nchar(mpl_handle handl);
long        mpl_set_nchar(long nchar, mpl_handle handl);
int         mpl_load_matrix(char* matrix, mpl_handle handl);
//          Set character types
//          Exclude character
//          Include character
//          Add taxon label
//          Delete taxon label
//          Exclude taxon
//          Include taxon
//          Set character weight
//          Get character weight
//          Set equal weights
//          Set character parsimony type
//          Get character parsimony type
//          Apply character settings
//          Set hold value
//          Get hold value
//          Set maxtrees
//          Get maxtrees
//          Set autoincrease value
//          Get autoincrease value
//          Set addition sequence type
//          Get addition sequence type
//          Set branch-swapping type
//          Get branch-swapping type
//          Get trees by stepwise addition
//          Get trees AS+branch-swapping

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* mpl_h */
