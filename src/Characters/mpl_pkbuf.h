//
//  mpl_pkbuf.h
//  morphy_nui_v
//
//  Created by Martin Brazeau on 19/06/2021.
//

#ifndef mpl_pkbuf_h
#define mpl_pkbuf_h

#include "mpl_chardefs.h"

#define MASK2S32  0x55555555
#define MASK4S32  0x11111111
#define MASK8S32  0x1010101
#define MASK16S32 0x10001
// TODO: conditionally compile on machine arch
// TODO: 32 and 64 state sets?

typedef struct mpl_pkbuf {
//    discr_t data;
} mpl_pkbuf;

#endif /* mpl_pkbuf_h */
