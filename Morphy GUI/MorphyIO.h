//
//  MorphyIO.h
//  Morphy
//
//  Created by Martin Brazeau on 03/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef MorphyIO_hp
#define MorphyIO_hp

#ifdef __cplusplus
extern "C" {
#endif
    
    //    typedef void* mplNexusReader;
    
    void* morphy_gui_file_open(const char* filename);
    void morphy_gui_get_taxon(char* labeldest, unsigned int taxnum, void* inrdr);
    int morphy_gui_get_ntax(void* inrdr);
    
    
#ifdef __cplusplus
}
#endif
#endif /* MorphyIO_hp */
