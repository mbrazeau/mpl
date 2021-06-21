//
//  mpl_charinfo.h
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_charinfo_h
#define mpl_charinfo_h

#include <stdbool.h>

#include "mpl_defs.h"
#include "mpl_chardefs.h"
#include "mpl_charbuf.h"

typedef struct _charinfo {
    
    long         index;
    int          num_states;
    mpl_discr    states;
    int          maxstate;
    int          minstate;
    long         num_gaps;
    mpl_data_t   datatype;
    mpl_parsim_t parsimtype;
    bool         isincluded;
    bool         isparsinform;
    bool         isvariable;
    double       weight;
    double*      wtptr; // Points to the weight in the derived char buf.
    double       preweight; // Probability of reweighting
    long         depends[MPL_MAX_DEPENDECIES];
    long         nsubchars;
    long*        subchars;
    mpl_charbuf* destbuf; // Buffer this character ended up in
    long         bufindex; // Index in destination buffer
    
} mpl_charinfo;

void mpl_charinfo_set_defaults(mpl_charinfo* ci);
void mpl_charinfo_set_data_t(const mpl_data_t datype, mpl_charinfo* ci);
mpl_data_t mpl_charinfo_get_data_t(mpl_charinfo* ci);
void mpl_charinfo_set_parsim_t(const mpl_parsim_t ptype, mpl_charinfo* ci);
mpl_parsim_t mpl_charinfo_get_parsim_t(const mpl_charinfo* ci);
void mpl_charinfo_add_state_count(const int n, mpl_charinfo* ci);
void mpl_charinfo_set_statecount(const int n, mpl_charinfo* ci);
void mpl_charinfo_include(mpl_charinfo* ci);
void mpl_charinfo_exclude(mpl_charinfo* ci);
bool mpl_charinfo_isincluded(mpl_charinfo* ci);
void mpl_charinfo_set_weight(double wt, mpl_charinfo* ci);
double mpl_charinfo_get_weight(mpl_charinfo* ci);
void mpl_charinfo_set_states(mpl_discr d, mpl_charinfo *ci);
mpl_discr mpl_charinfo_get_states(mpl_charinfo *ci);

#endif /* mpl_charinfo_h */
