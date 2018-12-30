//
//  mpltest.h
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpltest_h
#define mpltest_h

#include <stdio.h>
#include <stdlib.h>

#define pfail \
printf("[  FAIL  ] %s, line: %i in: %s\n", __FUNCTION__, __LINE__, __FILE__)

#define ppass printf("[  PASS  ] %s\n", __FUNCTION__)

#define psumf(...) \
printf("[  ** FAILED ** ] %i times. Review output to see details.\n", fails)

#define psump printf("[  ** PASSED ** ] All tests passed.\n")

#define theader(testname) printf("\n\n\t%s\n\n", testname);


#endif /* mpltest_h */
