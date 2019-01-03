//
//  testio.cpp
//  Morphy GUI
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include "testio.h"
#include "ncl.h"

void* mpl_test_file_open(int argnum, const char* argv[])
{
    MultiFormatReader *reader = new MultiFormatReader();
    
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
    
    reader->ReadFilepath(argv[argnum], MultiFormatReader::NEXUS_FORMAT);
    
    return (void*)reader;
}

void rdr_get_taxon(char* labeldest, unsigned int taxnum, void* inrdr)
{
    MultiFormatReader* nxsrdr = (MultiFormatReader*)inrdr;
    NxsTaxaBlock *taxa = nxsrdr->GetTaxaBlock(0);
    NxsString label;
    label = taxa->GetTaxonLabel(taxnum);
    size_t len = label.length();
    label.copy(labeldest, len);
    labeldest[len] = '\0';
}

int rdr_get_ntax(void* inrdr)
{
    MultiFormatReader* nxsrdr = (MultiFormatReader*)inrdr;
    
    return nxsrdr->GetTaxaBlock(0)->GetNTax();
}
