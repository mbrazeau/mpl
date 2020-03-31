//
//  mpl_cluster.h
//  morphy_nui_v
//
//  Created by Martin Brazeau on 29/03/2020.
//

#ifndef mpl_cluster_h
#define mpl_cluster_h

#include <stdbool.h>

typedef struct _bitset  mpl_bitset;
typedef struct _cluster mpl_cluster;

typedef struct _cluster {
    
    int             size;   // Number of tips subtended by this cluster
    int             count;  // Number of identical clusters (for consensus trees)
    mpl_bitset*     bitset; // Bitset defining the bipartition
    mpl_cluster*    next;   // For linked listing
    
} mpl_cluster;

mpl_cluster*    mpl_cluster_new(const int ntax);
void            mpl_cluster_delete(mpl_cluster** clust);

/**
 @brief Joins two cluster in a bitwise OR operation.
 
 @discussion Forms the bitwise union of two clusters and updates the count of
 taxa incorporated into the cluster.
 
 @param dest    A pointer to the destination result cluster.
 @param clust1  A pointer to the first source cluster.
 @param clust2  A pointer to the second source cluster.
 */
void            mpl_cluster_join(mpl_cluster*       dest,
                                 const mpl_cluster* clust1,
                                 const mpl_cluster* clust2);

/**
 @brief Compares two clusters for compatibility.
 
 @discussion This function determines if a cluster is compatible with the
 supplied constraint. 
 
 @param clust   The cluster to be checked against the constraint.
 @param constr  The constraint cluster.
 @return True if compatible, false if not compatible.
 */
bool            mpl_cluster_compat(const mpl_cluster* clust,
                                   const mpl_cluster* constr);

#endif /* mpl_cluster_h */
