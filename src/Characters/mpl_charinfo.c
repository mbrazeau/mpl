//
//  mpl_charinfo.c
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "mpl_charinfo.h"

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
