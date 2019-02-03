//
//  mpl_search.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//
#include <assert.h>

#include "mpl_search.h"
#include "mpl_bbreak.h"
#include "mpl_stepwise.h"



mpl_results* mpl_do_search(mpl_search* s)
{
    
    s->searchfxn(s);
    
    // TODO: For now:
    return NULL;
}
