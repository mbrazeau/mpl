//
//  testio.hpp
//  Morphy GUI
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef testio_hpp
#define testio_hpp

#ifdef __cpluslus
extern "C" {
#endif

typedef void* mplNexusReader;
    
mplNexusReader* file_open(int argc, const char* argv[]);
void  rdr_get_taxon(char* res, long taxon_number, mplNexusReader* nr);

    
#ifdef __cplusplus
}
#endif

#endif /* testio_hpp */
