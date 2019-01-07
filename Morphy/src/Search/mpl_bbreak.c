//
//  mpl_bbreak.c
//  Morphy
//
//  Created by Martin Brazeau on 07/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "mpl_bbreak.h"

//void mpl_do_bbreak(mpl_tree* t, mpl_bbreak* bbk)
//{
//    long ntax = 0; // The number of taxa in the tree
//    long nnodes = 0; // The number of nodes in the tree
//    mpl_node** nodes = t->postord_all; // The postorder list of nodes in the tree
//    mpl_node* srcs = *bbk->srcs;
//    mpl_node* src = srcs;
//    mpl_node* tgts = *bbk->tgts;
//    mpl_node* tgt = tgts;
//    
//    long i = 0;
//    
//    for (i = 1; i < nnodes; ++i) { // Assumes 'unrooted' tree
//        // Clip the tree at i
//        
//        // <<< Check the cost >>> of reinserting the node at this clip
//        // If the result is no difference on the length of the
//        // tree, this branch is zero-length and can be skipped
//        
//        // For each rerooting site
//        while (src != srcs) {
//            
//            // Re-root the tree
//            
//            // Try all target sites
//            // for each target site <<< check the length >>> of the tree {
//            //  Sort storage or discarding of try based on length
//            //  Put the src tree back in its original spot
//            
//            ++src;
//        }
//        
//        // Restore the tree to exactly as it was before
//    }
//}
