//
//  mpl_error.h
//  Morphy
//
//  Created by Martin Brazeau on 02/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_error_h
#define mpl_error_h

typedef enum {
    MPL_SUCCESS         =  0,
    MPL_ERR             = -1,
    MPL_NOMEM           = -2,
    MPL_OUTOFBOUNDS     = -3,
    MPL_UNEXPNULLPTR    = -4,
    MPL_NOTIMPLEMENTED  = -5
} MPL_RETURN;

#endif /* mpl_error_h */
