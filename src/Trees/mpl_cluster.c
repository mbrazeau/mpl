//
//  mpl_cluster.c
//  morphy_nui_v
//
//  Created by Martin Brazeau on 29/03/2020.
//

#include <assert.h>
#include <stdlib.h>

#include "mpl_utils.h"
#include "mpl_bitset.h"
#include "mpl_cluster.h"

mpl_cluster* mpl_cluster_new(const int ntax)
{
    assert(ntax >= 3); // Not much point creating a cluster with less than 3 taxa
    
    mpl_cluster* newclust = NULL;
    
    newclust = (mpl_cluster*)safe_calloc(1, sizeof(mpl_cluster));
    
    if (newclust != NULL) {
        newclust->bitset = mpl_bitset_new(ntax);
    }
    
    return newclust;
}

void mpl_cluster_delete(mpl_cluster** clust)
{
    mpl_cluster* iclust = *clust;
    
    if (clust != NULL) {
        mpl_bitset_delete(&iclust->bitset);
        iclust->next = NULL;
        iclust->size = 0;
        
        free(*clust);
        *clust = NULL;
    }
}

void mpl_cluster_join
(mpl_cluster*       dest,
const mpl_cluster*  clust1,
const mpl_cluster*  clust2)
{
    mpl_bitset_OR(dest->bitset, clust1->bitset, clust2->bitset);
    dest->size = clust1->size + clust2->size;
}

bool mpl_cluster_compat(const mpl_cluster* clust, const mpl_cluster* constr)
{
#ifdef DEBUG
    assert(clust->size == constr->size);
#endif
    return mpl_bitset_compat(clust->bitset, constr->bitset);
}
