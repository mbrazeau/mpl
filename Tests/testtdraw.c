//
//  testtdraw.c
//  Morphy
//
//  Created by Martin Brazeau on 17/03/2020.
//  Copyright © 2020 Martin Brazeau. All rights reserved.
//

#include "testutils.h"
#include "mpltest.h"

#include "testtdraw.h"
#include "mpl_tdraw.h"
#include "mpl_taxblock.h"
#include "mpl_newick_rdr.h"

int test_new_tree_drawing (void)
{
    theader("Test allocating and setup of a new tree drawing");
    
    int failn = 0;
    
    int ntax = 10;
    
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
//    char* nwkstring = "(1,(2,(3,(4,(5,(6,(7,(8,(9,10)))))))));";
    char* nwkstring = "(1,((7,(8,(9,10),5,4,6)),2,3));";
//    char* nwkstring = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    mpl_taxablock *tb = mpl_taxablock_new(ntax);
    
    int i = 0;
    for (i = 0; i < ntax; ++i) {
        mpl_taxablock_add_taxon(taxa[i], tb);
    }
    
    mpl_tdraw* tdrw = mpl_tdraw_new(tb);
    
    if (tdrw == NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    printf("\n");

    mpl_tree* t = mpl_new_tree(ntax);
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_init(ntax, &top);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    mpl_newick_read(nwkstring, &top, &rdr);
    
    mpl_tree_read_topol(t, &top);
    
    t->num_polys = 20;
    mpl_test_print_newick(t->base);
    printf("\n");
    
    mpl_tdraw_do(t, tdrw);
    
    for (i = 0; i < tdrw->height; ++i) {
        printf("%s", tdrw->canvas[i]);
    }
    
    mpl_tdraw_delete(&tdrw);
    
    return failn;
}

int test_large_tree_drawing (void)
{
    theader("Test printing of large tree");
    
    int failn = 0;
    
    int ntax = 95;
    
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
    
//    char* nwkstring = "(1,(2,(((8,(72,((21,((95,(28,((45,(((63,(43,(((13,(3,40)),(65,(12,54))),(75,(26,33))))),(31,(49,((9,(46,(69,(27,(((17,(4,19)),((39,(15,23)),(38,(59,76)))),(16,(60,74))))))),(62,(68,((7,18),(77,(37,(52,57)))))))))),(22,71))),(94,(((79,(50,(24,(14,((41,(56,(55,90))),(47,(88,89))))))),(73,(((30,35),((81,(34,64)),(80,(82,(83,86))))),((58,91),(87,(84,85)))))),(93,(36,(66,92)))))))),(48,(25,(10,(20,42)))))),(29,((70,(5,51)),(11,53)))))),(32,44)),(78,(61,(6,67))))));";
    char* nwkstring = "((1,2),(((((6,67),61,78),8),((5,11,70),29,(51,53))),72),(((((((((((((3,40),13),(54,65)),12,31),(43,63)),(26,33)),(22,75)),(((((((((((((4,19),17),(((15,23),39),((38,59),76))),(60,74)),16),27),69),57),37),52),(((7,(62,77)),18),68)),(9,46)),49)),71),((((((14,(41,(((47,(88,89)),56),55,90))),24,79),(((((30,35),(((34,64),81),(80,(82,(83,86))))),73),((58,91),((84,85),87))),94)),(36,((66,92),93))),50),45)),95),(32,44)),((((10,(20,42)),21),25),48)),28);";
    
    mpl_taxablock *tb = mpl_taxablock_new(ntax);
    
    int i = 0;
    for (i = 0; i < ntax; ++i) {
        mpl_taxablock_add_taxon(taxa[i], tb);
    }
    
    mpl_tdraw* tdrw = mpl_tdraw_new(tb);
    
    if (tdrw == NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    printf("\n");

    mpl_tree* t = mpl_new_tree(ntax);
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_init(ntax, &top);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    mpl_newick_read(nwkstring, &top, &rdr);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_tdraw_do(t, tdrw);
    
    for (i = 0; i < tdrw->height; ++i) {
        printf("%s", tdrw->canvas[i]);
    }
    
    mpl_tdraw_delete(&tdrw);
    
    return failn;
}

int test_drawing_collapsed (void)
{
    theader("Test collapsed tree");
    
    int failn = 0;
    
    int ntax = 5;
    
    char* taxa[] = {
        "Alpha",
        "Beta",
        "Gamma",
        "Delta",
        "Epsilon",
    };
    
    char* nwkstring = "((1,5),((2,4),3));";
    
    mpl_taxablock *tb = mpl_taxablock_new(ntax);
    
    int i = 0;
    for (i = 0; i < ntax; ++i) {
        mpl_taxablock_add_taxon(taxa[i], tb);
    }
    
    mpl_tdraw* tdrw = mpl_tdraw_new(tb);
    
    if (tdrw == NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    printf("\n");

    mpl_tree* t = mpl_new_tree(ntax);
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_init(ntax, &top);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    mpl_newick_read(nwkstring, &top, &rdr);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_node_collapse(t->nodes[1].anc);
    t->num_polys = 1;
    mpl_tdraw_do(t, tdrw);
    
    for (i = 0; i < tdrw->height; ++i) {
        printf("%s", tdrw->canvas[i]);
    }
    
    mpl_tdraw_delete(&tdrw);
    
    return failn;
}
