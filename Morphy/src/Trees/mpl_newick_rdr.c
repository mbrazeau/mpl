//
//  mpl_newick_rdr.c
//  Morphy
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <string.h>
#include <ctype.h>

//TODO: Temporary:
#include <stdio.h>

#include "../Taxa/mpl_taxblock.h"
#include "mpl_newick_rdr.h"
#include "mpl_utils.h"

static int mpl_newick_verify(const char* newick_str, const long num_taxa, mpl_newick_rdr* rdr);
static long mpl_newick_traverse
(char** ncur, long* index, long* place, long* edgetab, mpl_newick_rdr* rdr);

int mpl_newick_rdr_init(long num_taxa, mpl_newick_rdr* rdr)
{
    int ret = -1;
    int i = 0;
    
    if (rdr) {
        ret = 0;
        rdr->taxnamemax = MPLTAXNAMEMAX;
        if (!(rdr->namebuffer = (char*)safe_calloc(MPLTAXNAMEMAX, sizeof(char)))) {
            ret = -2;
        }
        if (!(rdr->tree = mpl_new_tree(num_taxa))) {
            free(rdr->namebuffer);
            rdr->namebuffer = 0;
            ret = -2;
        }
    }
    
    return ret;
}

int mpl_newick_read(const char* nwkstr, mpl_topol* top, mpl_newick_rdr* rdr)
{
    // For instance:    ((1,5),(3,(2,4)))
    // indices  0  1  2  3  4  5  6  7  8
    // Target: {5, 6, 7, 6, 5, 8, 7, 8, -1};
    
    int ret = 0;
    
    if (!nwkstr || !top || !rdr) {
        return -1;
    }
    
    if ((ret = mpl_newick_verify(nwkstr, top->num_taxa, rdr))) {
        return ret;
    }
    
    long i = 0;
    long p = 2 * top->num_taxa - 2;
    long q = p;
    long end = mpl_newick_traverse((char**)&nwkstr, &i, &p, top->edges, rdr);
    //top->edges[end] = -1;
    rdr->tree->base = &rdr->tree->nodes[end];
    mpl_record_topol(top, rdr->tree);
    return ret;
}

static int mpl_newick_verify(const char* newick_str, const long num_taxa, mpl_newick_rdr* rdr)
{
    long open = 0;
    long close = 0;
    long comma = 0;
    long space = 0;
    char* c = (char*)newick_str;
    char l = 0;
    
    while (*c) {
        if      (*c == '(') ++open;
        else if (*c == ')') ++close;
        else if (*c == ',') ++comma;
        else if (*c == ' ') ++space;
        l = *c;
        ++c;
    }
    
    rdr->nopenbr = open;
    rdr->nclosebr = close;
    rdr->ncomma = comma;
    rdr->nspace = space;
    
    if (open != close) {
        return -1;
    }
    else if (comma != num_taxa - 1) {
        return -2;
    }
    else if (l != ';') {
        return -3;
    }
    
    return 0;
}

static long mpl_newick_traverse
(char** ncur, long* index, long* place, long* edgetab, mpl_newick_rdr* rdr)
{
    long pos = *place;
    long desc = 0;
    
    if (**ncur == ';') {
        return -1;
    }
    
    while (**ncur == ' ') ++ncur;
    
    ++(*ncur);
    
    do {
        if (**ncur == '(') {
            //++(*ncur);
            --(*place);
            desc = mpl_newick_traverse(ncur, index, place, edgetab, rdr);
            printf("%li: ", desc);
            printf("%li; ", pos);
            //edgetab[desc] = pos;
            mpl_tree_push_desc(pos, desc, rdr->tree);
        }
        
        while (**ncur == ' ') ++(*ncur);
        
        if (isalnum(**ncur)) {
            // Read token
            printf("%c: ", **ncur-1);
            //edgetab[**ncur-'0'-1] = pos;
            mpl_tree_push_desc(pos, **ncur-'0'-1, rdr->tree);
            printf("%li; ", pos);
            do {
                ++(*ncur);
            } while (isalnum(**ncur));
        }
        
        if (**ncur == ',') {
            ++(*index);
            ++(*ncur);
        }
        
    } while (**ncur != ')' && **ncur != ';');
    
    ++(*ncur);
    
    return pos;
}
