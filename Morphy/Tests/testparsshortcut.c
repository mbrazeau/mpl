//
//  testparsshortcut.c
//  Morphy
//
//  Created by Martin Brazeau on 28/05/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//


/*
 Stuff for testing the parsimony shortcut being used.
 */

#include "testharddat.h"
#include "testparsshortcut.h"
#include "testmplbbreak.h"
#include "../src/Trees/mpl_node.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "../src/Trees/mpl_newick_rdr.h"
#include "../src/Characters/mpl_matrix.h"
#include "../src/Analysis/mpl_scoretree.h"
#include "../src/Analysis/mpl_stepwise.h"

int test_parsimony_shortcut (void)
{
    theader("Test parsimony shortcut with inapplicable data");
    
    int failn = 0;
    
    long ntax = Miyashita.ntax;
    long nchar = Miyashita.nchar;
    double testscore = 0.0;
    
    char* matrix = Miyashita.chardat;
    
    // Ohmdenia
//    char* testtree = "(1,(7,(((25,(16,(2,6))),(15,(17,(14,24)))),((9,((11,(12,(19,((26,((3,(5,(23,(13,18)))),28)),(4,22))))),(20,27))),(21,(8,10))))));";
    // Miyashita et al.
    char* testtree = "(1,(4,(2,(3,(5,((7,(6,8)),(11,((((10,(23,(22,(9,(21,(25,((24,(30,(28,29))),(26,27)))))))),(12,((13,(18,19)),((14,16),(15,(17,20)))))),(37,(38,(36,(32,(31,(34,(33,35)))))))),(((39,40),(41,42)),((43,(44,45)),(47,(46,(48,(51,(52,(49,50))))))))))))))));";
    
    mpl_matrix* m = NULL;
    
    // Setup the input data
    m = mpl_matrix_new();
    mpl_matrix_init(ntax, nchar, 2 * nchar, m);
    mpl_matrix_attach_rawdata(matrix, m);
    mpl_matrix_apply_data(m);
    mpl_init_parsimony(m);
    
    // Setup the input tree
    mpl_tree* t = NULL;
    t = mpl_new_tree(ntax);
    
    // Setup a newick reader
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    // Setup a topology for the newick reader
    mpl_topol* top = NULL;
    top = mpl_topol_new(ntax);
    
    mpl_newick_read(testtree, top, &rdr);

    // Set up the tree according to the input newick
    mpl_tree_read_topol(t, top);
    
    // Calculate the length of the tree given the data and parameters
    testscore = mpl_fullpass_parsimony(t);
    printf("\nScore of the whole tree: %.0f\n", testscore);
    // Store this as a test length
    
    mpl_tree_traverse(t);
    long i = 0;
    long maxnodes = t->size;
    double tgtlen = 0.0;
    double srclen = 0.0;
    double reclen = 0.0;
    mpl_node* src = NULL;
    mpl_node* site = NULL;
    mpl_node* left = NULL;
    mpl_node* right = NULL;
    
    
    // Perform all clippings on the tree
    // For each clipping...
    for (i = 1; i < maxnodes-2; ++i) { // TODO: May be able to use -1
        
        // Clip the tree:
        src = t->postord_all[i];
        site = mpl_node_get_sib(src);

        if (src->anc->left == src) {
            right = site;
            left = NULL; // Simply precautionary
        }
        else {
            left = site;
            right = NULL;
        }
        
        mpl_node_bin_clip(src);
        
        // Calculate the length of each subtree
        tgtlen = 0.0;
        srclen = 0.0;
        tgtlen = mpl_fullpass_parsimony(t);
        if (src->tip == 0) {
            srclen = mpl_fullpass_subtree(src, t);
        }
        
        // Reconnect at clipsite:
        mpl_node_bin_connect(left, right, src);
        
        // Calculate the length of reinserting to same spot
        reclen = mpl_score_try_parsimony(tgtlen + srclen, -1.0, src, site, t);

        reclen += tgtlen + srclen;
        // Compare with test length
        
        if (reclen != testscore) {
            ++failn;
            pfail;
        }
        else {
            ppass;
        }
    }


    return failn;
}

int test_parsimony_shortcut_with_rerooting (void)
{
    theader("Test parsimony shortcut with inapplicable data and rerooted subtree");
    
    int failn = 0;
    
    //    int ntax = 28;
    //    int nchar = 95;
    long ntax = Miyashita.ntax;
    long nchar = Miyashita.nchar;
    double testscore = 0.0;
    
    char* matrix = Miyashita.chardat;
    

    // Ohmdenia
    //    char* testtree = "(1,(7,(((25,(16,(2,6))),(15,(17,(14,24)))),((9,((11,(12,(19,((26,((3,(5,(23,(13,18)))),28)),(4,22))))),(20,27))),(21,(8,10))))));";
    // Miyashita et al.
    char* testtree = "(1,(4,(2,(3,(5,((7,(6,8)),(11,((((10,(23,(22,(9,(21,(25,((24,(30,(28,29))),(26,27)))))))),(12,((13,(18,19)),((14,16),(15,(17,20)))))),(37,(38,(36,(32,(31,(34,(33,35)))))))),(((39,40),(41,42)),((43,(44,45)),(47,(46,(48,(51,(52,(49,50))))))))))))))));";
    
    double testscores[] = {379.0, 379.0, 379.0, 379.0, 379.0, 379.0, 379.0, 379.0, 379.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 380.0, 381.0, 381.0, 381.0, 381.0, 381.0, 380.0, 381.0, 381.0, 381.0, 381.0, 381.0, 382.0, 382.0, 383.0, 383.0, 383.0, 383.0, 382.0, 383.0, 383.0, 384.0, 384.0, 384.0, 384.0, 385.0, 385.0, 383.0, 385.0, 384.0, 384.0, 384.0, 386.0, 386.0, 385.0, 386.0, 386.0, 386.0, 386.0, 386.0, 386.0, 386.0, 385.0, 386.0, 386.0, 391.0, 391.0, 393.0, 393.0, 394.0, 392.0, 393.0, 393.0, 392.0, 393.0, 393.0, 393.0, 391.0, 393.0, 393.0, 396.0, 396.0, 396.0, 396.0, 396.0, 397.0, 397.0, 397.0, 398.0, 397.0, 397.0, 396.0, 396.0, 397.0, 391.0, 396.0, 396.0, 395.0, 396.0, 395.0, 392.0, 397.0, 397.0, 397.0, 396.0, 396.0, 396.0, 395.0, 396.0, 394.0, 395.0, 394.0, 394.0, 392.0, 394.0, 394.0, 399.0, 399.0, 399.0, 399.0, 399.0, 398.0, 400.0, 400.0, 400.0, 400.0, 401.0, 400.0, 402.0, 401.0, 401.0, 402.0, 402.0, 398.0, 402.0, 400.0, 400.0, 400.0, 400.0, 400.0, 400.0, 400.0, 401.0, 400.0, 400.0, 400.0, 400.0, 400.0, 400.0, 400.0, 400.0, 400.0, 403.0, 403.0, 403.0, 404.0, 404.0, 404.0, 403.0, 404.0, 404.0, 404.0, 404.0, 404.0, 406.0, 406.0, 404.0, 404.0, 406.0, 406.0, 404.0, 404.0, 404.0, 404.0, 404.0, 409.0, 409.0, 409.0, 409.0, 409.0, 409.0, 409.0, 409.0, 409.0, 409.0, 410.0, 410.0, 410.0, 409.0, 409.0, 418.0, 418.0, 416.0, 415.0, 433.0, 433.0, 419.0, 419.0, 433.0, 434.0, 418.0, 414.0, 434.0, 434.0, 420.0, 420.0, 434.0, 434.0, 419.0, 420.0, 434.0, 434.0, 417.0, 417.0, 433.0, 434.0, 417.0, 417.0, 430.0, 433.0, 416.0, 417.0, 416.0, 414.0, 416.0, 416.0, 414.0, 416.0, 416.0, 416.0, 416.0, 416.0, 416.0, 416.0, 416.0, 416.0, 416.0, 416.0, 417.0, 416.0, 416.0, 425.0, 425.0, 430.0, 430.0, 430.0, 427.0, 426.0, 419.0, 426.0, 421.0, 419.0, 419.0, 422.0, 422.0, 427.0, 427.0, 429.0, 430.0, 436.0, 434.0, 434.0, 465.0, 465.0, 472.0, 472.0, 463.0, 472.0, 463.0, 462.0, 457.0, 463.0, 463.0, 469.0, 468.0, 468.0, 472.0, 471.0, 468.0, 471.0, 479.0, 469.0, 471.0, 463.0, 478.0, 478.0, 481.0, 468.0, 493.0, 493.0, 469.0, 456.0, 492.0, 485.0, 473.0, 472.0, 493.0, 493.0, 466.0, 473.0, 493.0, 492.0, 467.0, 466.0, 491.0, 491.0, 463.0, 468.0, 491.0, 491.0, 458.0, 460.0, 491.0, 493.0, 494.0, 494.0, 494.0, 492.0, 487.0, 492.0, 492.0, 491.0, 476.0, 492.0, 482.0, 482.0, 495.0, 495.0, 495.0, 495.0, 496.0, 496.0, 497.0, 497.0, 497.0, 497.0, 497.0, 497.0, 497.0, 494.0, 494.0, 494.0, 494.0, 494.0, 494.0, 494.0, 494.0, 494.0, 495.0, 509.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 504.0, 509.0, 510.0, 510.0, 515.0, 515.0, 512.0, 516.0, 516.0, 533.0, 534.0, 541.0, 541.0, 543.0, 542.0, 542.0, 552.0, 555.0, 558.0, 560.0, 551.0, 558.0, 540.0, 550.0, 550.0, 543.0, 549.0, 549.0, 538.0, 555.0, 550.0, 552.0, 557.0, 542.0, 545.0, 545.0, 543.0, 544.0, 554.0, 555.0, 552.0, 546.0, 553.0, 554.0, 557.0, 559.0, 553.0, 555.0, 551.0, 547.0, 551.0, 550.0, 550.0, 556.0, 556.0, 551.0, 554.0, 556.0, 559.0, 553.0, 553.0, 558.0, 551.0, 554.0, 554.0, 565.0, 556.0, 564.0, 562.0, 559.0, 558.0, 547.0, 551.0, 550.0, 549.0, 549.0, 549.0, 549.0, 554.0, 554.0, 562.0, 562.0, 556.0, 556.0, 575.0, 575.0, 554.0, 555.0, 577.0, 577.0, 565.0, 565.0, 577.0, 577.0, 569.0, 569.0, 577.0, 577.0, 569.0, 571.0, 577.0, 577.0, 571.0, 568.0, 577.0, 577.0, 567.0, 569.0, 570.0, 577.0, 568.0, 569.0, 566.0, 570.0, 564.0, 570.0, 582.0, 582.0, 566.0, 564.0, 584.0, 584.0, 565.0, 564.0, 583.0, 580.0, 565.0, 566.0, 584.0, 584.0, 583.0, 583.0, 581.0, 581.0, 581.0, 581.0, 581.0, 584.0, 584.0, 583.0, 583.0, 582.0, 574.0, 582.0, 575.0, 582.0, 574.0, 578.0, 572.0, 574.0, 571.0, 574.0, 575.0, 575.0, 576.0, 576.0, 580.0, 580.0, 573.0, 577.0, 577.0, 585.0, 586.0, 586.0, 577.0, 589.0, 590.0, 581.0, 589.0, 581.0, 579.0, 582.0, 582.0, 580.0, 583.0, 577.0, 580.0, 588.0, 588.0, 588.0, 580.0, 589.0, 589.0, 590.0, 590.0, 586.0, 586.0, 587.0, 587.0, 589.0, 587.0, 589.0, 589.0, 588.0, 589.0, 584.0, 589.0, 589.0, 585.0, 586.0, 572.0, 576.0, 576.0, 583.0, 583.0, 583.0, 583.0, 583.0, 583.0, 583.0, 583.0, 583.0, 583.0, 583.0, 581.0, 581.0, 568.0, 565.0, 568.0, 569.0, 568.0, 571.0, 571.0, 571.0, 571.0, 578.0, 583.0, 583.0, 586.0, 582.0, 585.0, 580.0, 583.0, 581.0, 582.0, 581.0, 580.0, 580.0, 575.0, 574.0, 576.0, 574.0, 575.0, 575.0, 574.0, 574.0, 585.0, 585.0, 574.0, 574.0, 586.0, 586.0, 574.0, 574.0, 587.0, 584.0, 583.0, 582.0, 591.0, 590.0, 590.0, 589.0, 592.0, 590.0, 594.0, 590.0, 594.0, 591.0, 593.0, 591.0, 581.0, 588.0, 581.0, 581.0, 579.0, 581.0, 574.0, 579.0, 575.0, 575.0, 584.0, 584.0, 586.0, 586.0, 586.0, 586.0, 586.0, 586.0, 586.0, 586.0, 586.0, 586.0, 579.0, 586.0, 577.0, 579.0, 588.0, 589.0, 591.0, 591.0, 590.0, 586.0, 591.0, 591.0, 592.0, 591.0, 590.0, 590.0, 589.0, 590.0, 592.0, 589.0, 591.0, 592.0, 590.0, 591.0, 590.0, 583.0, 581.0, 590.0, 584.0, 584.0, 586.0, 587.0, 585.0, 586.0, 585.0, 583.0, 593.0, 593.0, 590.0, 594.0, 590.0, 581.0, 582.0, 589.0, 589.0, 600.0, 599.0, 596.0, 598.0, 597.0, 594.0, 603.0, 604.0, 599.0, 603.0, 599.0, 597.0, 602.0, 602.0, 600.0, 603.0, 599.0, 601.0, 607.0, 607.0, 607.0, 600.0, 608.0, 608.0, 609.0, 609.0, 606.0, 606.0, 607.0, 607.0, 609.0, 607.0, 608.0, 608.0, 607.0, 608.0, 608.0, 603.0, 607.0, 608.0, 595.0, 608.0, 597.0, 597.0, 602.0, 602.0, 602.0, 602.0, 602.0, 603.0, 602.0, 602.0, 602.0, 602.0, 600.0, 602.0, 592.0, 600.0, 592.0, 592.0, 598.0, 598.0, 600.0, 600.0, 606.0, 600.0, 614.0, 603.0, 613.0, 612.0, 613.0, 607.0, 611.0, 606.0, 613.0, 612.0, 614.0, 611.0, 609.0, 608.0, 605.0, 602.0, 602.0, 600.0, 602.0, 602.0, 595.0, 602.0, 594.0, 593.0, 594.0, 594.0, 594.0, 597.0, 596.0, 597.0, 597.0, 600.0, 599.0, 610.0, 611.0, 611.0, 611.0, 613.0, 609.0, 615.0, 608.0, 628.0, 621.0, 627.0, 626.0, 626.0, 628.0, 628.0, 622.0, 627.0, 623.0, 626.0, 624.0, 620.0, 619.0, 617.0, 617.0, 615.0, 617.0, 609.0, 615.0, 611.0, 611.0, 617.0, 617.0, 619.0, 619.0, 619.0, 619.0, 619.0, 619.0, 619.0, 619.0, 619.0, 619.0, 617.0, 619.0, 615.0, 617.0, 626.0, 627.0, 629.0, 629.0, 628.0, 624.0, 629.0, 629.0, 630.0, 629.0, 628.0, 628.0, 627.0, 628.0, 630.0, 627.0, 629.0, 630.0, 628.0, 629.0, 628.0, 621.0, 618.0, 628.0, 620.0, 620.0, 622.0, 623.0, 619.0, 622.0, 619.0, 617.0, 623.0, 623.0, 617.0, 624.0, 616.0, 614.0, 619.0, 618.0, 609.0, 620.0, 620.0, 631.0, 631.0, 631.0, 624.0, 632.0, 632.0, 634.0, 628.0, 642.0, 642.0, 637.0, 643.0, 637.0, 631.0, 640.0, 640.0, 634.0, 641.0, 633.0, 635.0, 646.0, 646.0, 646.0, 641.0, 647.0, 647.0, 647.0, 647.0, 644.0, 644.0, 645.0, 645.0, 647.0, 645.0, 647.0, 646.0, 647.0, 647.0, 648.0, 645.0, 647.0, 648.0, 636.0, 647.0, 637.0, 637.0, 639.0, 639.0, 639.0, 639.0, 639.0, 639.0, 639.0, 639.0, 639.0, 639.0, 639.0, 639.0, 636.0, 639.0, 637.0, 636.0, 643.0, 643.0, 644.0, 644.0, 649.0, 644.0, 653.0, 644.0, 652.0, 651.0, 651.0, 645.0, 649.0, 644.0, 651.0, 653.0, 652.0, 649.0, 643.0, 646.0, 640.0, 637.0, 638.0, 635.0, 637.0, 638.0, 628.0, 637.0, 626.0, 627.0, 625.0, 626.0, 625.0, 627.0};
    
    mpl_matrix* m = NULL;
    
    // Setup the input data
    m = mpl_matrix_new();
    mpl_matrix_init(ntax, nchar, 2 * nchar, m);
    mpl_matrix_attach_rawdata(matrix, m);
    mpl_matrix_apply_data(m);
    mpl_init_parsimony(m);
    
    // Setup the input tree
    mpl_tree* t = NULL;
    t = mpl_new_tree(ntax);
    
    // Setup a newick reader
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    // Setup a topology for the newick reader
    mpl_topol* top = NULL;
    top = mpl_topol_new(ntax);
    
    mpl_newick_read(testtree, top, &rdr);
    
    // Set up the tree according to the input newick
    mpl_tree_read_topol(t, top);
    
    // Calculate the length of the tree given the data and parameters
    testscore = mpl_fullpass_parsimony(t);
    printf("\nScore of the whole tree: %.0f\n", testscore);
    // Store this as a test length
    
    mpl_tree_traverse(t);
    long i = 0;
    long maxnodes = t->size;
    double tgtlen = 0.0;
    double srclen = 0.0;
    double reclen = 0.0;
    mpl_node* src = NULL;
    mpl_node* site = NULL;
    mpl_node* left = NULL;
    mpl_node* right = NULL;
    
    
    // Perform all clippings on the tree
    // For each clipping...
    int c = 0;
    for (i = 1; i < maxnodes-2; ++i) { // TODO: May be able to use -1
        
        // Clip the tree:
        src = t->postord_all[i];
        site = mpl_node_get_sib(src);
        
        if (src->anc->left == src) {
            right = site;
            left = NULL; // Simply precautionary
        }
        else {
            left = site;
            right = NULL;
        }
        
        mpl_node_bin_clip(src);
        
        // Calculate the length of each subtree
        tgtlen = 0.0;
        srclen = 0.0;
        tgtlen = mpl_fullpass_parsimony(t);
        if (src->tip == 0) {
            srclen = mpl_fullpass_subtree(src, t);
        }
        
        mpl_node* rtlef = NULL;
        mpl_node* rtrig = NULL;
        mpl_node* oldnbr = NULL;
        mpl_node* newnbr = NULL;
        mpl_node** srcs = (mpl_node**)safe_calloc(2 * ntax, sizeof(mpl_node*));
        mpl_node** srcbuf = srcs;
        mpl_node** rt = NULL;
        
        // Set up the src pointers
        if (src->tip == 0) {
            rtlef = src->left;
            rtrig = src->right;
            *srcs = src->left;
            ++srcs;
            if (src->right->tip == 0) {
                *srcs = src->right->left;
                ++srcs;
                *srcs = src->right->right;
                ++srcs;
            }
        }
        else {
            *srcs = src;
            ++srcs;
        }
        
        // Perform all rerootings
        

        for (rt = srcbuf; rt < srcs; ++rt) {
            
            mpl_test_bbreak_tbr_reroot(*rt, src);
            
            newnbr = *rt;
            
            if ((*rt)->tip == 0) {
                *srcs = (*rt)->left;
                ++srcs;
                *srcs = (*rt)->right;
                ++srcs;
            }
            
            if (src->tip == 0) {
                if (oldnbr != NULL) {
                    if (oldnbr->tip > 0) {
                        oldnbr = oldnbr->anc;
                    }
                    mpl_update_src_actives(oldnbr, src);
                }
            }
            mpl_src_root_parsimony(src);
            
            oldnbr = newnbr;
            // Calculuate the length of reinsertion to same spot
            // Compare with test length
            // Reconnect at clipsite:
            
            mpl_node_bin_connect(left, right, src);

            // Calculate the length of reinserting to same spot
            reclen = mpl_score_try_parsimony(tgtlen + srclen, -1.0, src, site, t);

            reclen += tgtlen + srclen;
            // Compare with test length
            //printf("%.1f, ", mpl_fullpass_parsimony(t));
            if (reclen != testscores[c]) {
                ++failn;
                pfail;
            }
            else {
                ppass;
            }
            
            ++c;
            
            mpl_node_bin_clip(src);
                                       
        }
        
        mpl_node_bin_connect(left, right, src);

        free(srcbuf);
        
    }
    
    //printf("\nScores: %i", c);
    
    return failn;
}
