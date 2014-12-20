include_directories(..)
file(GLOB HEADERS "*.h")
aux_source_directory(. SRC_LIST)
set(EXECUTABLE testkad)
add_executable(${EXECUTABLE} ${SRC_LIST} ${HEADERS})
target_link_libraries(${EXECUTABLE} kademlia)

