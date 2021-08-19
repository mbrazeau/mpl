//
//  mpl_contree.h
//  morphy_nui_v03
//
//  Created by Martin Brazeau on 19/08/2021.
//

#ifndef mpl_contree_h
#define mpl_contree_h

typedef struct _treelist mpl_treelist;
typedef struct _tree mpl_tree;
typedef struct mpl_bipartlist mpl_bipartlist;

typedef struct _contree {
    mpl_tree        *t;
    mpl_treelist    *treelist;
    mpl_bipartlist  *biparts;
} mpl_contree;

#endif /* mpl_contree_h */
