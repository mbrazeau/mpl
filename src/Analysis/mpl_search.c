//
//  mpl_search.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//
#include <assert.h>

#include "mpl_search.h"
#include "mpl_bbreak.h"
#include "mpl_stepwise.h"

#define DEFAULT_SEARCH_T MPL_HEURISTIC_T

mpl_search* mpl_search_new(const long ntax, mpl_treelist* tl)
{
    mpl_search* s = NULL;
    
    s = (mpl_search*)safe_calloc(1, sizeof(mpl_search));
    
    if (s != NULL) {
        s->num_taxa    = ntax;
        s->search_type = DEFAULT_SEARCH_T;
        s->bbreak_type = DEFAULT_BBREAK;
        s->stepw_type  = DEFAULT_ASTYPE;
        s->ratch_iter  = DEFAULT_RATCHITER;
        s->num_reps    = DEFAULT_NREPS;
        s->use_ratchet = false;
        s->hold        = 1;
        s->keep        = 0; // 0 keepscore implies keep all
        s->treelist    = NULL;
        s->searchfxn   = NULL;
        s->tree        = NULL;
        s->constraint  = NULL;
        
        s->tree = mpl_new_tree(s->num_taxa);
        
        if (s->tree == NULL) {
            safe_free(s);
            return NULL;
        }
        
        s->treelist = tl;
        s->bbreak   = mpl_bbreak_new(0);
        
        mpl_search_set_search_t(s->search_type, s);
    }
    
    return s;
}

void mpl_search_delete(mpl_search** s)
{
    if (s == NULL || *s == NULL) {
        return;
    }
    
    mpl_delete_tree(&(*s)->tree);
    
    safe_free(*s);
}

int mpl_search_set_search_t(const mpl_search_t stype, mpl_search* s)
{
    RET_IF_NULL(s);
    
    MPL_RETURN ret = MPL_ERR;
    
    s->search_type = stype;

    switch (s->search_type) {
        case MPL_HEURISTIC_T:
            s->searchfxn = mpl_bbreak_search;
            ret = MPL_SUCCESS;
            break;
        default:
            ret = MPL_NOTIMPLEMENTED;
            break;
    }
    
    return ret;
}

int mpl_search_set_bbreak_t(const mpl_bbreak_t bbkt, mpl_search* s)
{
    RET_IF_NULL(s);
    
    s->bbreak_type = bbkt;
    
    return MPL_SUCCESS;
}

int mpl_search_set_stepw_t(const mpl_addseq_t swtype, mpl_search* s)
{
    RET_IF_NULL(s);
    
    s->stepw_type = swtype;
    
    return MPL_SUCCESS;
}

int mpl_search_set_hold(const int hold, mpl_search* s)
{
    RET_IF_NULL(s);
    
    s->hold = hold;
    
    return MPL_SUCCESS;
}

int mpl_search_add_treelist(mpl_treelist* tl, mpl_search* s)
{
    RET_IF_NULL(s);
    
    s->treelist = tl;
    
    return MPL_SUCCESS;
}

int mpl_search_set_numreps(const long nreps, mpl_search* s)
{
    RET_IF_NULL(s);
    
//    s->bbreak->numreps = nreps;
    s->num_reps = nreps;

    return MPL_SUCCESS;
}

bool mpl_search_is_reptype(mpl_search* s)
{
    bool ret = false;
    switch (s->search_type) {
        case MPL_HEURISTIC_T:
            switch (s->stepw_type) {
                case MPL_AST_ASIS:
                    ret = false;
                    break;
                case MPL_AST_INMEM:
                    ret = false;
                    break;
                case MPL_AST_RANDOM:
                    ret = true;
                    break;
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
    return ret;
}

int mpl_search_use_ratchet(const bool useratchet, mpl_search* s)
{
    s->use_ratchet = useratchet;
    return 0;
}

MPL_RETURN mpl_search_execute(mpl_search* s)
{
    MPL_RETURN ret = MPL_ERR;
    
    ret = s->searchfxn(s);

    return ret;
}

/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/

int mpl_search_init(mpl_search* s)
{
    assert(s != NULL);
    
    // Set all defaults
    
    return MPL_ERR;
}
