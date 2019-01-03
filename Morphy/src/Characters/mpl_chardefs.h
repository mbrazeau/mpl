//
//  mpl_chardegs.h
//  Morphy
//
//  Created by Martin Brazeau on 02/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_chardegs_h
#define mpl_chardegs_h

#include <limits.h>
#include <stdlib.h>

typedef unsigned long   mpl_discr;

#define NA      ((mpl_discr)1UL)
#define MISSING ((mpl_discr)~0UL)
#define UNKNOWN ((mpl_discr)~NA)

#define MPLMAXSTATES (CHAR_BIT * sizeof(mpl_discr) - 1UL)  // -1 for the NA reserved token


#endif /* mpl_chardegs_h */
