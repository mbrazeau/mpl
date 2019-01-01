//
//  mpl_topol.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "mpl_topol.h"
#include "../mpl_utils.h"

static int mpl_initialise_topology(long num_taxa, mpl_topol* top);

/*
 *  PUBLIC FUNCTION DEFINITIONS
 */

mpl_topol*  mpl_topol_new(long num_taxa)
{
    if (num_taxa == 0) {
        return NULL;
    }
    
    mpl_topol* newtop = NULL;
    
    if (newtop) {
        if (mpl_initialise_topology(num_taxa, newtop)) {
            // If this fails, then delete new topology and exit.
            mpl_topol_delete(&newtop);
            return newtop;
        }
    }
    
    return newtop;
}

int  mpl_topol_delete(mpl_topol** top)
{
    if (!top) {
        return -1;
    }
    if (!*top) {
        return -1;
    }
    
    if ((*top)->edges != NULL) {
        free((*top)->edges);
        (*top)->edges = NULL;
    }
    
    free(*top);
    *top = NULL;
    
    return 0;
}

int mpl_topol_reset(long num_taxa, mpl_topol* top)
{
    if (top && num_taxa) {
        
        int i = 0;
        
        top->num_polys = 0;
        top->root = -1;
        
        // TODO: At this point the function could check numtaxa
        // matches the old numtaxa and, if not, call back to
        // topol_initialise to fix it. This should not end up in an
        // infinite cycle.
        if (num_taxa != top->num_taxa) {
            mpl_initialise_topology(num_taxa, top);
        }
        
        for (i = 0; i < top->num_nodes; ++i) {
            top->edges[i] = -1;
        }
        
        top->next = NULL;
        top->back = NULL;
    }
        
    return -1;
}

int mpl_topol_link(mpl_topol* parent, mpl_topol* child)
{
    if (parent && child) {
        
        parent->next = child;
        child->back = parent;
        
        return 0;
    }
    
    return -1;
}

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

static int mpl_initialise_topology(long num_taxa, mpl_topol* top)
{
    int ret = 0;
    
    if (!top) {
        return -1;
    }
    
    top->num_taxa = num_taxa;
    top->num_nodes = 2 * num_taxa - 1;
    
    if (top->edges != NULL) {
        free(top->edges);
        top->edges = NULL;
    }
    
    top->edges = (long*)safe_calloc(top->num_nodes, sizeof(long));
    if (!top->edges) {
        ret = -1;
    }
    
    mpl_topol_reset(num_taxa, top);
    
    return ret;
}


