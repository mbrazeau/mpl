//
//  mpl_tdraw.c
//  Morphy
//
//  Created by Martin Brazeau on 20/04/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

//
//
//      /--- Name 2
//  /---+
//  |   \--- Name 1
//  +
//  \------- Name 3
//

// Canvas:
// cccc....ccc\n
// cccc....ccc\n
// cccc....ccc\n\0;

#include <stdio.h>

#include "mpl_tdraw.h"
#include "mpl_utils.h"

static void calc_dimensions(const long ntax, mpl_tdraw *td);

mpl_tdraw* mpl_tdraw_new(const mpl_taxablock *taxa)
{
    long i = 0;
    long j = 0;
    mpl_tdraw *newtd = NULL;
    
    newtd = safe_calloc(1, sizeof(mpl_tdraw));
    
    if (newtd != NULL) {
        newtd->taxa = taxa;
        calc_dimensions(taxa->num_taxa, newtd);
        
        // Size the canvas based on the calculated display dimensions.
        // Add 1 to the width for the null character at the end of each line.
        // Add 1 to the total for the terminating null character.
        newtd->canvas = (char**)alloc_matrix(newtd->height,
                                             newtd->width + 2,
                                             sizeof(char));
        
        if (newtd->canvas != NULL) {
            // Lay out the canvas as a bunch of spaces, newlines
            for (i = 0; i < newtd->height; ++i) {
                for (j = 0; j <= newtd->width; ++j) {
                    newtd->canvas[i][j] = ' ';
                }
                newtd->canvas[i][j-1] = '\n';
                newtd->canvas[i][j]   = '\0';
            }
        }
    }
    
    return newtd;
}

void mpl_tdraw_delete(mpl_tdraw **td)
{
    long i = 0;
    
    for (i = 0; i < (*td)->height; ++i) {
        safe_free((*td)->canvas[i]);
    }
    
    (*td)->taxa = NULL;
    
    safe_free(*td);
    
    *td = NULL;
    
    return;
}

void mpl_tdraw_do(mpl_tree *t, mpl_tdraw *td)
{
    
}

/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/

static void calc_dimensions(const long ntax, mpl_tdraw* td)
{
    td->width = DEFAULT_CANVAS_WIDTH;
    td->height = 2 * ntax - 1;
}
