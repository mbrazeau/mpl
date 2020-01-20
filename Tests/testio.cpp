//
//  testio.cpp
//  Morphy GUI
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <string.h>
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

int rdr_get_nchar(void* inrdr)
{
    MultiFormatReader* nxsrdr = (MultiFormatReader*)inrdr;
    
    return nxsrdr->GetCharactersBlock(nxsrdr->GetTaxaBlock(0), 0)->GetNChar();
}

void rdr_get_matrix_notaxlabels(char** dest, void* inrdr)
{
    MultiFormatReader* nxsrdr = (MultiFormatReader*)inrdr;
    
    std::string matrix;
    
    NxsCharactersBlock* cblock = nxsrdr->GetCharactersBlock(nxsrdr->GetTaxaBlock(0), 0);
    int i = 0;
    for (i = 0; i < cblock->GetNumMatrixRows(); ++i) {
        matrix.append(cblock->GetMatrixRowAsStr(i));
    }
    matrix.append(";");
    *dest = NULL;
    *dest = (char*)malloc((matrix.size() + 1) * sizeof(char));
    if (*dest == NULL) {
        std::cout << "Out of memory!\n";
        exit(EXIT_FAILURE);
    }
    
    strcpy(*dest, matrix.c_str());
}

void rdr_get_matrix_symbols(char** dest, void* inrdr)
{
    MultiFormatReader* nxsrdr = (MultiFormatReader*)inrdr;
    
    std::string symbols;
    
    NxsCharactersBlock* cblock = nxsrdr->GetCharactersBlock(nxsrdr->GetTaxaBlock(0), 0);
    
    symbols = cblock->GetSymbols();
    
    std::cout << "Successfully read symbols: " << symbols << std::endl;
    
    *dest = NULL;
    *dest = (char*)malloc((symbols.size() + 1) * sizeof(char));
    if (*dest == NULL) {
        std::cout << "Out of memory!\n";
        exit(EXIT_FAILURE);
    }
    
    strcpy(*dest, symbols.c_str());
}
