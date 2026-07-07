file(GLOB_RECURSE COMMON_SOURCES *.cpp *.c *.h *.hpp)

add_library(common ${COMMON_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${COMMON_SOURCES})

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common PUBLIC glad glm dds glfw assimp compressonator stb)