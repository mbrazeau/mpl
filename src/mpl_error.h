//
//  mpl_error.h
//  morphy_nui_v
//
//  Created by Martin Brazeau on 21/08/2021.
//

#ifndef mpl_error_h
#define mpl_error_h

typedef enum {
    
    MPL_SUCCESS         =  0,  // No error
    MPL_ERR             = -1,  // General error
    MPL_NOMEM           = -2,  // Insufficient memory
    MPL_OUTOFBOUNDS     = -3,  // Subscript or index is out of buffer's bounds
    MPL_UNEXPNULLPTR    = -4,  // Function expects non-NULL pointer
    MPL_NOTIMPLEMENTED  = -5,  // The requested routine is not implemented
    MPL_NODIMENSIONS    = -6,  // Input matrix has no known dimensions
    MPL_ILLEGOVERWRITE  = -7,  // Attempt to inexplicitly overwrite data
    MPL_BADPARAM        = -8,  // Some inappropriate parameter passed to the function
    MPL_NOSEMICOLON     = -9,  // Expected terminal semicolon in input
    MPL_PARENTHMSM      = -10, // Mismatched parentheses
    MPL_BADDIMENSIONS   = -11, // Input matrix has dimensions error
    MPL_BADSYMBOL       = -12, // Input uses unrecognised symbol
    MPL_NOTREES         = -13, // There are no trees in memory
    MPL_NOTREADY        = -14  // Dataset not ready for attempted operation
    
} MPL_RETURN;

char *mpl_new_errbuf(void);
void mpl_delete_errbuf(char **buf);
void mpl_parse_error(char *charbuf, MPL_RETURN err);

#endif /* mpl_error_h */
