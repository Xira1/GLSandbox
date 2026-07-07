cmake_minimum_required(VERSION 3.20)
project(GLSandbox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)
set(RUNTIME_DATA_DIR ${CMAKE_SOURCE_DIR}/../runtime_data)

add_compile_definitions(
    RUNTIME_DATA="${RUNTIME_DATA_DIR}"
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

# Helper-macro prebuilt with single .lib on all cfgs 
macro(add_prebuilt_lib NAME LIB_PATH INCLUDE_PATH)
    add_library(${NAME} STATIC IMPORTED GLOBAL)
    set_target_properties(${NAME} PROPERTIES
        IMPORTED_LOCATION             ${LIB_PATH}
        IMPORTED_LOCATION_DEBUG       ${LIB_PATH}
        IMPORTED_LOCATION_RELEASE     ${LIB_PATH}
        IMPORTED_LOCATION_RELWITHDEBINFO ${LIB_PATH}
        IMPORTED_LOCATION_MINSIZEREL  ${LIB_PATH}
        INTERFACE_INCLUDE_DIRECTORIES ${INCLUDE_PATH}
    )
endmacro()

# Vendors
add_library(glad STATIC ${VENDOR_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC ${VENDOR_DIR}/glad/include)

add_library(stb STATIC ${VENDOR_DIR}/stb_image/stb_image.cpp)
target_include_directories(stb PUBLIC ${VENDOR_DIR}/stb_image)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${VENDOR_DIR}/glm)

add_library(dds STATIC ${VENDOR_DIR}/DDS/DDS_Helpers.cpp)
target_include_directories(dds PUBLIC ${VENDOR_DIR}/DDS)

# Vendor: prebuilt
# GLFW
add_library(glfw STATIC IMPORTED GLOBAL)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION_DEBUG          ${VENDOR_DIR}/glfw/lib/Debug/glfw3.lib
    IMPORTED_LOCATION_RELEASE        ${VENDOR_DIR}/glfw/lib/Release/glfw3.lib
    IMPORTED_LOCATION_RELWITHDEBINFO ${VENDOR_DIR}/glfw/lib/Release/glfw3.lib
    IMPORTED_LOCATION_MINSIZEREL     ${VENDOR_DIR}/glfw/lib/Release/glfw3.lib
    INTERFACE_INCLUDE_DIRECTORIES    ${VENDOR_DIR}/glfw/include
)

add_prebuilt_lib(assimp
    ${VENDOR_DIR}/assimp/lib/assimp-vc143-mt.lib
    ${VENDOR_DIR}/assimp/include
)
add_prebuilt_lib(compressonator
    ${VENDOR_DIR}/compressonator/lib/Compressonator_MD.lib
    ${VENDOR_DIR}/compressonator/include
)

set_target_properties(glad stb dds PROPERTIES FOLDER "Vendor")

add_subdirectory(common)
add_subdirectory(main)