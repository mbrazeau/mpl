//
//  MorphyIO.cpp
//  Morphy GUI
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//
#include "ncl.h"
#include "MorphyIO.h"

void* morphy_gui_file_open(const char* filename)
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
    
    reader->ReadFilepath(filename, MultiFormatReader::NEXUS_FORMAT);
    
    return (void*)reader;
}

void morphy_gui_get_taxon(char* labeldest, unsigned int taxnum, void* inrdr)
{
    MultiFormatReader* nxsrdr = (MultiFormatReader*)inrdr;
    NxsTaxaBlock *taxa = nxsrdr->GetTaxaBlock(0);
    NxsString label;
    label = taxa->GetTaxonLabel(taxnum);
    size_t len = label.length();
    label.copy(labeldest, len);
    labeldest[len] = '\0';
}

int morphy_gui_get_ntax(void* inrdr)
{
    MultiFormatReader* nxsrdr = (MultiFormatReader*)inrdr;
    
    return nxsrdr->GetTaxaBlock(0)->GetNTax();
}
