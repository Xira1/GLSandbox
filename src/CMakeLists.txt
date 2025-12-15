cmake_minimum_required(VERSION 3.20)
project(GLSandbox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sandbox)

set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(VENDOR_DIR ${PROJECT_ROOT}/vendor)
set(RUNTIME_DATA_DIR ${PROJECT_ROOT}/../runtime_data)
set(DLL_DIR ${VENDOR_DIR}/DLLs)

# glad
add_library(glad STATIC ${VENDOR_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC ${VENDOR_DIR}/glad/include)

# stb
add_library(stb STATIC ${VENDOR_DIR}/stb_image/stb_image.cpp)
target_include_directories(glad PUBLIC ${VENDOR_DIR}/stb_image)

# glm 
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${VENDOR_DIR}/glm)

# dds (cpp + h)
add_library(dds STATIC ${VENDOR_DIR}/DDS/DDS_Helpers.cpp)
target_include_directories(dds PUBLIC ${VENDOR_DIR}/dds)

# GLFW 
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION_DEBUG   ${VENDOR_DIR}/glfw/lib/Debug/glfw3.lib
    IMPORTED_LOCATION_RELEASE ${VENDOR_DIR}/glfw/lib/Release/glfw3.lib
    IMPORTED_LOCATION_RELWITHDEBINFO ${VENDOR_DIR}/glfw/lib/Release/glfw3.lib
    IMPORTED_LOCATION_MINSIZEREL ${VENDOR_DIR}/glfw/lib/Release/glfw3.lib
    INTERFACE_INCLUDE_DIRECTORIES ${VENDOR_DIR}/glfw/include
)

# Assimp 
add_library(assimp STATIC IMPORTED)
set_target_properties(assimp PROPERTIES
    IMPORTED_LOCATION_DEBUG   ${VENDOR_DIR}/assimp/lib/assimp-vc143-mt.lib
    IMPORTED_LOCATION_RELEASE ${VENDOR_DIR}/assimp/lib/assimp-vc143-mt.lib
    IMPORTED_LOCATION_RELWITHDEBINFO ${VENDOR_DIR}/assimp/lib/assimp-vc143-mt.lib
    IMPORTED_LOCATION_MINSIZEREL ${VENDOR_DIR}/assimp/lib/assimp-vc143-mt.lib
    INTERFACE_INCLUDE_DIRECTORIES ${VENDOR_DIR}/assimp/include
)

# Compressonator 
add_library(compressonator STATIC IMPORTED)
set_target_properties(compressonator PROPERTIES
    IMPORTED_LOCATION_DEBUG   ${VENDOR_DIR}/compressonator/lib/Compressonator_MD.lib
    IMPORTED_LOCATION_RELEASE ${VENDOR_DIR}/compressonator/lib/Compressonator_MD.lib
    IMPORTED_LOCATION_RELWITHDEBINFO ${VENDOR_DIR}/compressonator/lib/Compressonator_MD.lib
    IMPORTED_LOCATION_MINSIZEREL ${VENDOR_DIR}/compressonator/lib/Compressonator_MD.lib
    INTERFACE_INCLUDE_DIRECTORIES ${VENDOR_DIR}/compressonator/include
)

add_compile_definitions(
    RUNTIME_DATA="${RUNTIME_DATA_DIR}"
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

set_target_properties(glad PROPERTIES FOLDER "Vendor")
set_target_properties(dds PROPERTIES FOLDER "Vendor")
set_target_properties(stb PROPERTIES FOLDER "Vendor")

add_subdirectory(${PROJECT_ROOT}/common)
add_subdirectory(${PROJECT_ROOT}/main)