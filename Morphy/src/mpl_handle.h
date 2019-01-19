//
//  mpl_handle.h
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_handle_h
#define mpl_handle_h

typedef void* mpl_handle;

mpl_handle* mpl_handle_new(void);
// TODO: Use error codes
long        mpl_handle_get_ntax(mpl_handle handl);
int         mpl_handle_set_ntax(long ntax, mpl_handle handl);
int         mpl_handle_get_nchar(mpl_handle handl);
long        mpl_handle_set_nchar(long nchar, mpl_handle handl);
int         mpl_handle_load_matrix(char* matrix, mpl_handle handl);
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



#endif /* mpl_handle_h */
