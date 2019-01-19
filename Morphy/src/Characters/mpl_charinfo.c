//
//  mpl_charinfo.c
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "mpl_charinfo.h"

void mpl_charinfo_set_defaults(mpl_charinfo* ci)
{
    ci->datatype        = DEFAULT_DATA_T;
    ci->parsimtype      = DEFAULT_PARSIM_T;
    ci->isincluded      = true;
    ci->isparsinform    = false;
}
