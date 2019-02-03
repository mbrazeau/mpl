//
//  testio.hpp
//  Morphy GUI
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef testio_hpp
#define testio_hpp

#ifdef __cplusplus
extern "C" {
#endif

typedef void* mplNexusReader;
    
mplNexusReader mpl_test_file_open(int argnum, const char* argv[]);

void        rdr_get_taxon(char* labeldest, unsigned int taxnum, void* inrdr);
int         rdr_get_ntax(mplNexusReader nr);
int         rdr_get_nchar(mplNexusReader nr);
void        rdr_get_matrix_notaxlabels(char** dest, void* inrdr);
void        rdr_get_matrix_symbols(char** dest, void* inrdr);

    
#ifdef __cplusplus
}
#endif

#endif /* testio_hpp */
