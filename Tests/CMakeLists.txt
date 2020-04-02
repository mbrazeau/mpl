file(GLOB SRCS
	"*.h"
	"*.c"
	"*.cpp"
)

include_directories(../include ../src ../src/Analysis ../src/Characters ../src/Results ../src/Taxa ../src/Trees)

add_executable(morphytest ${SRCS})
target_link_libraries(morphytest ${PATH_LIB_NCL} mpl)
#target_link_libraries(morphytest m)

install(TARGETS morphytest DESTINATION bin)
