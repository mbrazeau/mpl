//
//  mpl_handle.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

//#include "mpl_handle.h"
#include <stdlib.h>

#include "mpl_defs.h"
#include "mpl_utils.h"

mpl_handle* mpl_handle_new(void)
{
    mpl_handle* newhandl = NULL;
    
    newhandl = (mpl_handle*)safe_calloc(1, sizeof(mpl_handle));
    
    return newhandl;
}


