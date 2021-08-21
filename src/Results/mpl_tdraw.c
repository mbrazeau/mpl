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
#include <assert.h>

#include "mpl_tree.h"
#include "mpl_tdraw.h"
#include "mpl_utils.h"

static void calc_dimensions(const long ntax, mpl_tdraw *td);
static void mpl_tdraw_fill_nodebar(mpl_node* n, mpl_tdraw *td);
static void mpl_tdraw_fill_subbranch(mpl_node *n, mpl_tdraw *td);
static void mpl_tdraw_fillin(mpl_tree *t, mpl_tdraw *td);
static int  mpl_tdraw_set_coords(mpl_tree *t, int *currentrow, int brlen);

mpl_tdraw* mpl_tdraw_new(mpl_taxablock *taxa)
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
}

void mpl_tdraw_do(mpl_tree *t, mpl_tdraw *td)
{
    int lastx = 0;
    int firstrow = 0;
    
    t->num_polys = 1; // TODO: needs to be a more elegant way to do this
    
    mpl_tree_ladderize(1, t);
    mpl_tree_traverse(t);
    
    // Set coordinates on the branches
    int brlen = DEFAULT_TIP_COLUMN / t->num_taxa;
    if (brlen == 0) {
        brlen = 3;
    }
    
//    if (brlen * )
    
    lastx = mpl_tdraw_set_coords(t, &firstrow, brlen);
    
    if (lastx > 0) {
          int i = 0;
          for (i = t->num_taxa; i < t->num_nodes; ++i) {
              t->nodes[i].x -= lastx;
          }
    } else if (lastx < 0) {
        firstrow = 0;
        // Uh oh...
        lastx = mpl_tdraw_set_coords(t, &firstrow, 1);
        assert(lastx > 0);
    }
    
    // Fillin the drawing
    mpl_tdraw_fillin(t, td);
}

/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/

static void calc_dimensions(const long ntax, mpl_tdraw* td)
{
    td->width  = DEFAULT_CANVAS_WIDTH;
    td->height = 2 * ntax - 1;
}

static void mpl_tdraw_write_tipname(mpl_node *n, mpl_tdraw *td)
{
    assert(n->tip);
    
    int i = 0;
    char *name = NULL;
    
    name = td->taxa->taxa[n->mem_index].name;
    
    if (td->taxa != NULL) {
        i = 0;
        // These conditions should prevent any line or buffer overflow
        while (td->canvas[n->y][n->x + i + 1] != '\n'
               && td->canvas[n->y][n->x + i + 1]
               && name[i]) {
            td->canvas[n->y][n->x + i + 1] = name[i];
            ++i;
        }
    }
}

static void mpl_tdraw_fill_subbranch(mpl_node *n, mpl_tdraw *td)
{
    int i = 0;
    mpl_node *anc = n->anc;
    
    for (i = 1; i < (n->x - anc->x); ++i) {
        td->canvas[n->y][anc->x + i] = '-';
    }
}

static void mpl_tdraw_fill_nodebar(mpl_node* n, mpl_tdraw *td)
{
    assert(n->tip == 0);
    
    int i = 0;
    
    td->canvas[n->y][n->x] = '+';
    td->canvas[n->left->y][n->x] = '/';
    td->canvas[n->right->y][n->x] = '\\';
    
    for (i = n->y+1; i < n->right->y; ++i) {
        td->canvas[i][n->x] = '|';
    }
    
    for (i = n->left->y+1; i < n->y; ++i) {
        td->canvas[i][n->x] = '|';
    }
}

static void mpl_tdraw_fillin(mpl_tree *t, mpl_tdraw *td)
{
    int i = 0;
    int j = 0;
    mpl_node *n = NULL;
    mpl_node *p = NULL;
    
    for (i = 0; i < t->size; ++i) {
        n = t->postord_all[i];
        
        if (n->tip) {
            // Write in the tip label if it's there
            mpl_tdraw_write_tipname(n, td);
            continue;
        }
        
        j = 0;
        do {
            p = n->descs[j];
            // Fill in the subbranch
            mpl_tdraw_fill_subbranch(p, td);
            ++j;
        } while (j < n->ndescs);
        
        // Put the node marker in and add the nodebar
        mpl_tdraw_fill_nodebar(n, td);
    }
    
}

static int mpl_tdraw_set_coords(mpl_tree *t, int *currentrow, int brlen)
{
    int i = 0;
    mpl_node* n = NULL;
    mpl_node** p;
    
    for (i = 0; i < t->size; ++i) {
        n = t->postord_all[i];
        if (n->tip) {
            // set the coords
            n->x = DEFAULT_TIP_COLUMN;
            n->y = *currentrow;
            *currentrow += 2;
//            continue;
        }
        else {
            n->left = n->descs[0];
            n->right = n->descs[n->ndescs-1];
            if (n->right->y > n->left->y) {
                n->y = n->left->y + ((n->right->y - n->left->y) / 2);
            } else {
                n->y = n->right->y + ((n->left->y - n->right->y) / 2);
            }
            
            p = n->descs;
            
            int deepest = (*p)->x;
            do {
                if ((*p)->x < deepest) {
                    deepest = (*p)->x;
                }
                ++p;
            } while (*p);
            
            n->x = deepest - brlen;
//            if (n->left->x < n->right->x) {
//                n->x = n->left->x - brlen;
//            } else {
//                n->x = n->right->x - brlen;
//            }
        }
        
    }
    
    return n->x;
}
