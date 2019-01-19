//
//  mpl_discchars.c
//  Morphy
//
//  Created by Martin Brazeau on 11/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "mpl_discchars.h"

double mpl_fitch_downpass
(const long left, const long right, long n, mpl_discrchars* m)
{
    long i;
    long end = m->end;
    mpl_discr** matrix = m->matrix;
    double cost = 0.0;
    
    for (i = m->start; i < end; ++i) {
        
        matrix[n][i] = matrix[left][i] & matrix[right][i];
        
        if (matrix[n][i] == 0) {
            matrix[n][i] = matrix[left][i] | matrix[right][i];
            cost += m->weights[i];
        }
    }
    
    return cost;
}

void mpl_fitch_uppass
(const long left, const long right, long n, long anc, mpl_discrchars* m)
{
    long i;
    long end = m->end;
    mpl_discr** matrix = m->matrix;
    mpl_discr   fin = 0;
    
    for (i = m->start; i < end; ++i) {
       
        fin = matrix[anc][i] & matrix[n][i];
        
        if (fin != anc) {
            if (matrix[left][i] & matrix[right][i]) {
                fin = (matrix[n][i] | (matrix[anc][i] &(matrix[left][i] | matrix[right][i])));
            }
            else {
                fin = matrix[n][i] | matrix[anc][i];
            }
        }

        matrix[n][i] = fin;
    }
}
