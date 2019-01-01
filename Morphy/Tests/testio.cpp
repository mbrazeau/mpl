//
//  testio.cpp
//  Morphy GUI
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include "testio.hpp"
#include "ncl.h"

void* file_open(int argc, const char* argv[])
{
    NxsReader *reader = new NxsReader();
    
    NxsTaxaBlock        *taxa       = new NxsTaxaBlock;
    NxsAssumptionsBlock *assumpts   = new NxsAssumptionsBlock(taxa);
    NxsCharactersBlock  *chars      = new NxsCharactersBlock(taxa, assumpts);
    NxsTreesBlock       *trees      = new NxsTreesBlock(taxa);
    NxsDataBlock        *data       = new NxsDataBlock(taxa, assumpts);
    
    reader->Add(taxa);
    reader->Add(assumpts);
    reader->Add(chars);
    reader->Add(trees);
    reader->Add(data);
    
    reader->ReadFilepath(argv[1]);
    
    return (void*)reader;
}
