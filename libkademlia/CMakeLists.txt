file(GLOB HEADERS "*.h")
aux_source_directory(. SRC_LIST)
set(EXECUTABLE kademlia)
add_library(${EXECUTABLE} ${SRC_LIST} ${HEADERS})


