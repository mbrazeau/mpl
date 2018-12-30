//
//  main.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <stdio.h>
#include "mpltest.h"
#include "testmplnode.h"
#include "testmpltree.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    int fails = 0;
    
    // Test mpl_node.c
    fails += test_node_new_delete();
    fails += test_push_node();
    fails += test_push_to_polynode();
    fails += test_pop_desc();
    fails += test_remove_descendant();
    fails += test_basic_bin_traversal();
    
    // Test mpl_tree.c
    fails += test_tree_assembly_from_topology();
    
    printf("\n\nTest summary:\n\n");
    if (fails) {
        psumf(fails);
    }
    else {
        psump;
    }
    printf("\n");
    
    return fails;
    
}
