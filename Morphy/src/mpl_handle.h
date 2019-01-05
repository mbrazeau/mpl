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
long        mpl_handle_get_ntax(void);
int         mpl_handle_set_ntax(long ntax);



#endif /* mpl_handle_h */
