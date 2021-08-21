//
//  testmplcontree.c
//  morphy_nui_v03
//
//  Created by Martin Brazeau on 19/08/2021.
//

#include "mpltest.h"
#include "testutils.h"
#include "testmpltreelist.h"
#include "mpl_tree.h"
#include "mpl_topol.h"
#include "mpl_newick_rdr.h"
#include "mpl_treelist.h"
#include "mpl_contree.h"
#include "mpl_bipartlist.h"
#include "mpl_bitset.h"
#include "mpl_tdraw.h"

void print_bits(mpl_bitset* bits);

int test_strict_consensus_tree (void)
{
    theader("Test strict consensus");
    
    int i = 0;
    int failn = 0;
    
    long ntax = 10;
    long numtrees = 6;
    
    char* taxa[] = {
        "Alpha",
        "Beta",
        "Gamma",
        "Delta",
        "Epsilon",
        "Eta",
        "Theta",
        "Omega",
        "Rho",
        "Zeta"
    };
    
    mpl_taxablock *tb = mpl_taxablock_new(ntax);
    for (i = 0; i < ntax; ++i) {
        mpl_taxablock_add_taxon(taxa[i], tb);
    }
    
//    long extension_rate = 1;
    char* newicks[] = {
        "((((1,((2,7),(5,9))),(4,8)),6),(3,10));",
        "((((1,((2,7),(5,9))),(4,8)),6),(3,10));",
        "((((1,((2,9),(5,7))),(4,8)),6),(3,10));",
        "((((4,((2,9),(5,7))),(1,8)),6),(3,10));",
        "((((4,((2,9),(5,7))),(1,8)),6),(3,10));",
        "((((8,4),(((5,9),(2,7)),1)),10),(6,3));"
    };
    
    mpl_topol* top = mpl_topol_new(ntax);
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    // NOTE: Here we set less than numtrees to be the maxtrees setting
    mpl_treelist* tl = mpl_treelist_new(ntax, 1, 1);
    
    char* nwkout = NULL;
    
    for (i = 0; i < numtrees; ++i) {
        mpl_newick_read(newicks[i], top, &rdr);
        mpl_tree_read_topol(t, top);
        mpl_tree_write_newick(&nwkout, t);
        printf("%s\n", nwkout);
        free(nwkout);
        nwkout = NULL;
        mpl_tree_checker(t);
        mpl_treelist_add_tree(false, t, tl);
        mpl_topol_reset(ntax, top);
    }
    
    // Delete top so that the pointer can be used to get from the treelist.
    mpl_topol_delete(&top);
    
    // Setup the consensus tree
    mpl_contree *ctree;
    
    ctree = mpl_contree_new(ntax, tl);
    mpl_contree_strict(ctree);
    
//    for (i = 0; i < ctree->biparts->num_splits; ++i) {
//        print_bits(ctree->biparts->biparts[i]);
//        printf("\n");
//    }
    
    printf("\n");
    mpl_tree_write_newick(&nwkout, ctree->t);
    printf("%s\n", nwkout);
    free(nwkout);
    
    mpl_tdraw* tdrw = mpl_tdraw_new(tb);
    
    ctree->t->num_polys = 1;
    mpl_tdraw_do(ctree->t, tdrw);
    
    for (i = 0; i < tdrw->height; ++i) {
        printf("%s", tdrw->canvas[i]);
    }
    
    mpl_tdraw_delete(&tdrw);
    
    // TODO: Delete contree memory
    mpl_delete_tree(&t);
    
    return failn;
}

int test_big_consensus_tree (void)
{
    theader("Test big consensus");
    
    int i = 0;
    int failn = 0;
    
    long ntax = 95;
    long numtrees = 2;
    
    char* taxa[] = {
        "Galeaspida",
        "Osteostraci",
        "Acanthodes",
        "Akmonistion",
        "Austroptyctodus",
        "Bothriolepis",
        "Brachyacanthus",
        "Brindabellaspis",
        "Brochoadmones",
        "Buchanosteus",
        "Campbellodus",
        "Cassidiceps",
        "Cheiracanthus",
        "Cheirolepis",
        "Chondrenchelys",
        "Cladodoides",
        "Cladoselache",
        "Climatius",
        "Cobelodus",
        "Coccosteus",
        "Cowralepis",
        "Culmacanthus",
        "Debeerius",
        "Dialipina",
        "Dicksonosteus",
        "Diplacanthus",
        "Doliodus",
        "Entelognathus",
        "Eurycaraspis",
        "Eusthenopteron",
        "Euthacanthus",
        "Gemuendina",
        "Gladiobranchus",
        "Glyptolepis",
        "Gogonasus",
        "Guiyu",
        "Gyracanthides",
        "Hamiltonichthys",
        "Helodus",
        "Homalacanthus",
        "Howqualepis",
        "Incisoscutum",
        "Ischnacanthus",
        "Jagorina",
        "Janusiscus",
        "Kathemacanthus",
        "Kentuckia",
        "Kujdanowiaspis",
        "Latviacanthus",
        "Ligulalepis",
        "Lunaspis",
        "Lupopsyrus",
        "Macropetalichthys",
        "Mesacanthus",
        "Mimipiscis",
        "Moythomasia",
        "Obtusacanthus",
        "Onychodus",
        "Onychoselache",
        "Orthacanthus",
        "Parayunnanolepis",
        "Parexus",
        "Poracanthodes",
        "Porolepis",
        "Promesacanthus",
        "Psarolepis",
        "Pterichthyodes",
        "Ptomacanthus",
        "Pucapampella",
        "Rhamphodopsis",
        "Ramirosuarezia",
        "Romundina",
        "Styloichthys",
        "Tamiobatis",
        "Tetanopsyrus",
        "Tristychius",
        "V_waynensis",
        "Yunnanolepis",
        "Meemannia",
        "Youngolepis",
        "Powichthys",
        "Diabolepis",
        "Dipterus",
        "Diplocercides",
        "Miguashaia",
        "Uranolophus",
        "Gavinia",
        "Lawrenciella",
        "Kansasiella",
        "Raynerius",
        "Qingmenodus",
        "Achoania",
        "Sparalepis",
        "Ptyctolepis",
        "Minjinia"
    };
    
    mpl_taxablock *tb = mpl_taxablock_new(ntax);
    for (i = 0; i < ntax; ++i) {
        mpl_taxablock_add_taxon(taxa[i], tb);
    }
    
//    long extension_rate = 1;
    char* newicks[] = {
        "(1,(2,(((8,(72,((21,((95,(28,((45,(((63,(43,(((13,(3,40)),(65,(12,54))),(75,(26,33))))),(31,(49,((9,(46,(69,(27,(((17,(4,19)),((39,(15,23)),(38,(59,76)))),(16,(60,74))))))),(62,(68,((7,18),(77,(37,(52,57)))))))))),(22,71))),(94,(((79,(50,(24,(14,((41,(56,(55,90))),(47,(88,89))))))),(73,(((30,35),((81,(34,64)),(80,(82,(83,86))))),((58,91),(87,(84,85)))))),(93,(36,(66,92)))))))),(48,(25,(10,(20,42)))))),(29,((70,(5,51)),(11,53)))))),(32,44)),(78,(61,(6,67))))));",
        "(1,(2,(((8,(72,((21,((95,(28,((45,(((63,(43,(75,(((13,(3,40)),(65,(12,54))),(26,33))))),(49,(31,(62,((7,(68,(77,(18,((69,(27,(16,(((17,(4,19)),((39,(15,23)),(38,(59,76)))),(60,74))))),(37,(52,57))))))),(9,46)))))),(22,71))),(94,(((79,(50,(24,(14,((41,(56,(55,90))),(47,(88,89))))))),(73,(((30,35),((81,(34,64)),(80,(82,(83,86))))),((58,91),(87,(84,85)))))),(93,(36,(66,92)))))))),(48,(25,(10,(20,42)))))),(29,((70,(5,51)),(11,53)))))),(32,44)),(78,(61,(6,67))))));"
    };
    
    mpl_topol* top = mpl_topol_new(ntax);
    mpl_tree* t = mpl_new_tree(ntax);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    // NOTE: Here we set less than numtrees to be the maxtrees setting
    mpl_treelist* tl = mpl_treelist_new(ntax, 1, 1);
    
    char* nwkout = NULL;
    
    for (i = 0; i < numtrees; ++i) {
        mpl_newick_read(newicks[i], top, &rdr);
        mpl_tree_read_topol(t, top);
        mpl_tree_write_newick(&nwkout, t);
        printf("%s\n", nwkout);
        free(nwkout);
        nwkout = NULL;
        mpl_tree_checker(t);
        mpl_treelist_add_tree(false, t, tl);
        mpl_topol_reset(ntax, top);
    }
    
    // Delete top so that the pointer can be used to get from the treelist.
    mpl_topol_delete(&top);
    
    // Setup the consensus tree
    mpl_contree *ctree;
    
    ctree = mpl_contree_new(ntax, tl);
    mpl_contree_strict(ctree);
    
//    for (i = 0; i < ctree->biparts->num_splits; ++i) {
//        print_bits(ctree->biparts->biparts[i]);
//        printf("\n");
//    }
    
    printf("\n");
    mpl_tree_write_newick(&nwkout, ctree->t);
    printf("%s\n", nwkout);
    free(nwkout);
    
    mpl_tdraw* tdrw = mpl_tdraw_new(tb);
    
    ctree->t->num_polys = 1;
    mpl_tdraw_do(ctree->t, tdrw);
    
    for (i = 0; i < tdrw->height; ++i) {
        printf("%s", tdrw->canvas[i]);
    }
    
    mpl_tdraw_delete(&tdrw);
    
    // TODO: Delete contree memory
    mpl_delete_tree(&t);
    
    return failn;
}
