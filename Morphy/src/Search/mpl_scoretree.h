//
//  mpl_scoretree.h
//  Morphy
//
//  Created by Martin Brazeau on 25/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_scoretree_h
#define mpl_scoretree_h

#include "../Characters/mpl_parsim.h"

void    mpl_init_parsimony(mpl_matrix* m);
double  mpl_fullpass_parsimony(mpl_tree* t);
double  mpl_partpass_parsimony(mpl_node* start, mpl_tree* t);

#endif /* mpl_scoretree_h */
