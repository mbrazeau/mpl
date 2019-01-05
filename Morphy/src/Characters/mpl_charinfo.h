//
//  mpl_charinfo.h
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_charinfo_h
#define mpl_charinfo_h

#include <stdbool.h>

#include "mpl_chardefs.h"

typedef struct _charinfo {
    
    int             num_states;
    int             num_gaps;
    mpl_data_t      datatype;
    mpl_parsim_t    parsimtype;
    bool            isincluded;
    bool            isparsinform;
    
} mpl_charinfo;

#endif /* mpl_charinfo_h */
