//
//  mpl_error.c
//  mpl
//
//  Created by Martin Brazeau on 21/08/2021.
//

#include <string.h>

#include "mpl_error.h"
#include "mpl_utils.h"

#define ERR_MAX_BUFFER 256

char *mpl_new_errbuf(void)
{
    return safe_calloc(ERR_MAX_BUFFER, sizeof(char));
}

void mpl_delete_errbuf(char **buf)
{
    if (buf != NULL) {
        if (*buf != NULL) {
            free(**buf);
            *buf = NULL;
        }
    }
}

void mpl_parse_error(char *charbuf, MPL_RETURN err)
{
    char *msg;
    switch (err) {
        case MPL_SUCCESS:
            msg = "No error.";
            break;
        case MPL_ERR:
            msg = "The program has encountered an unexpected error.";
            break;
        case MPL_NOMEM:
            msg = "Insufficient memory for the requested operation.";
            break;
        case MPL_OUTOFBOUNDS:
            msg = "Subscript out of bounds.";
            break;
        case MPL_UNEXPNULLPTR:
            msg = "Unexpected NULL pointer.";
            break;
        case MPL_NOTIMPLEMENTED:
            msg = "Requested operation has not been implemented.";
            break;
        case MPL_NODIMENSIONS:
            msg = "No dimensions supplied for data. Specify both ntax and nchar.";
            break;
        case MPL_ILLEGOVERWRITE:
            msg = "Requested operation would overwrite data.";
            break;
        case MPL_BADPARAM:
            msg = "Invalid parameter value passed to function.";
            break;
        case MPL_NOSEMICOLON:
            msg = "Input missing expected terminal semicolon.";
            break;
        case MPL_PARENTHMSM:
            msg = "Mismatched parentheses.";
            break;
        case MPL_BADDIMENSIONS:
            msg = "Incorrect data dimensions and input data size.";
            break;
        case MPL_BADSYMBOL:
            msg = "Input has unrecognized symbol.";
            break;
        case MPL_NOTREES:
            msg = "No trees in memory.";
            break;
        case MPL_NOTREADY:
            msg = "Dataset not ready for requested operation.";
            break;
            
        default:
            break;
    }
    
    strncpy(charbuf, msg, ERR_MAX_BUFFER * sizeof(char));
}
