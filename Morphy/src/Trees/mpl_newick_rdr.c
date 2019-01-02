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
(char** ncur, long* index, long nd, long* edgetab, mpl_newick_rdr* rdr);

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
    i = top->num_taxa-1;
    //long p = 2 * top->num_taxa - 2;
    long p = top->num_taxa;
    long end =
    mpl_newick_traverse((char**)&nwkstr, &i, p, top->edges, rdr);
    top->edges[end+1] = -1;
//    rdr->tree->base = &rdr->tree->nodes[end];
//    mpl_record_topol(top, rdr->tree);
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
(char** ncur, long* index, long parent, long* edgetab, mpl_newick_rdr* rdr)
{
    
    //long nd = *index-1; // Current node
    long desc = 0;
    long anc = -1;
    long s = 0;
    long nd;
    
    long loopcount = 0;
    nd = *index;
    
    do {
        if (**ncur == '(') {
            ++(*ncur);
            ++(*index);
            desc = mpl_newick_traverse(ncur, index, nd, edgetab, rdr);
            edgetab[desc] = nd;
        }
        
        ++loopcount;
        
        if (**ncur == ',') {
            ++(*ncur);
        }
        
        if (isalnum(**ncur)) {
            
            // Get the index of the tip label
            // Assign current node index at edgetab[indexoftip]
            size_t j = 0;
            while (isalnum(**ncur) && j < MPLTAXNAMEMAX) {
                rdr->namebuffer[j] = **ncur;
                ++(*ncur);
                ++j;
            }
            rdr->namebuffer[j] = '\0';
            char *end = &rdr->namebuffer[j];
            long ind = 0;
            ind = strtol(rdr->namebuffer, &end, 10);
            
            edgetab[ind-1] = nd;
        }
        
    } while (**ncur != ')' && **ncur != ';');
    
    ++(*ncur);
    
    return nd;
}
