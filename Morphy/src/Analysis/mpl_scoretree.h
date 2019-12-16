//
//  mpl_scoretree.h
//  Morphy
//
//  Created by Martin Brazeau on 25/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_scoretree_h
#define mpl_scoretree_h

#include "../Characters/mpl_parsim.h"

typedef void    (*mpl_clipreopt)(mpl_node* src, mpl_node* csite, mpl_tree* t);
typedef double  (*mpl_scoretry)(const double lim, mpl_node* src, mpl_node* tgt, mpl_tree* t);

MPL_RETURN    mpl_init_parsimony(mpl_matrix* m);
double  mpl_fullpass_parsimony(mpl_tree* t);
double  mpl_length_only_parsimony(const double lim, mpl_tree* t);
double  mpl_fullpass_subtree(mpl_node* subtr, mpl_tree* t);
void    mpl_update_src_actives(mpl_node* start, const mpl_node* end);
double  mpl_fullpass_parsimony_na_only(const double lim, mpl_node* start, mpl_tree* t);
double  mpl_partpass_parsimony(mpl_node* start, mpl_tree* t);
void    mpl_src_root_parsimony(mpl_node* src);
double  mpl_score_try_parsimony(const double, const double lim, mpl_node* src, mpl_node* tgt, mpl_tree* t);
void    mpl_scoretree_copy_original_characters(void);
void    mpl_scoretree_restore_original_characters(void);
void    mpl_tempset_stdtype(void);
void    mpl_tempreset_natype(void);
void    mpl_parsim_update_root_prupset(const long left, const long right, const long n, mpl_matrix* m);

void mpl_do_ratchet_weights(void);
void mpl_reset_std_weights(void);
#endif /* mpl_scoretree_h */
