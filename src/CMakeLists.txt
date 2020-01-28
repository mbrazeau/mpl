file(GLOB_RECURSE LIBSRCS
    "*.h"
    "*.hpp"
    "*.c"
)
include_directories(../include ./ ./Taxa ./Characters ./Trees ./Analysis ./Results)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIBSRCS})
add_library(mpl STATIC ${LIBSRCS})

install(TARGETS mpl DESTINATION lib)
install(DIRECTORY ../include/ DESTINATION include)

