//
//  mpl_charinfo.c
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "mpl_charinfo.h"

void mpl_charinfo_set_hilobit(mpl_charinfo *ci);

void mpl_charinfo_set_defaults(mpl_charinfo* ci)
{
    ci->datatype        = DEFAULT_DATA_T;
    ci->parsimtype      = DEFAULT_PARSIM_T;
    ci->isincluded      = true;
    ci->isparsinform    = false;
    ci->weight          = DEFAULT_WEIGHT;
    ci->preweight       = DEFAULT_PREWEIGHT;
}

void mpl_charinfo_set_data_t(const mpl_data_t datype, mpl_charinfo* ci)
{
    ci->datatype = datype;
}

mpl_data_t mpl_charinfo_get_data_t(mpl_charinfo* ci)
{
    return ci->datatype;
}

void mpl_charinfo_set_parsim_t(const mpl_parsim_t ptype, mpl_charinfo* ci)
{
    ci->parsimtype = ptype;
}

mpl_parsim_t mpl_charinfo_get_parsim_t(const mpl_charinfo* ci)
{
    return ci->parsimtype;
}

void mpl_charinfo_add_state_count(const int n, mpl_charinfo* ci)
{
    ci->num_states += n;
}

void mpl_charinfo_set_statecount(const int n, mpl_charinfo* ci)
{
    ci->num_states = n;
}

void mpl_charinfo_include(mpl_charinfo* ci)
{
    ci->isincluded = true;
}

void mpl_charinfo_exclude(mpl_charinfo* ci)
{
    ci->isincluded = false;
}

bool mpl_charinfo_isincluded(mpl_charinfo* ci)
{
    return ci->isincluded;
}

void mpl_charinfo_set_weight(double wt, mpl_charinfo* ci)
{
    ci->weight = wt;
}

double mpl_charinfo_get_weight(mpl_charinfo* ci)
{
    return ci->weight;
}

void mpl_charinfo_set_states(mpl_discr d, mpl_charinfo *ci)
{
    ci->states = d;
    mpl_charinfo_set_hilobit(ci);
}

mpl_discr mpl_charinfo_get_states(mpl_charinfo *ci)
{
    return ci->states;
}

/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/

/**
 Sets the range for the non-trivial states in the character.
 
 @param ci A pointer to the charinfo structure.
 @return void.
 */
void mpl_charinfo_set_hilobit(mpl_charinfo *ci)
{
    mpl_discr d = ci->states;
    
    ci->maxstate = 0;
    ci->minstate = 0;
    
    while (!(d & 1)) {
        d = d >> 1;
        ++ci->minstate;
        ++ci->maxstate;
    }
    
    while (d >> 1) {
        d = d >> 1;
        ++ci->maxstate;
    }
}
