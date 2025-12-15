project(sandbox)

file(GLOB_RECURSE MAIN_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.c *.h *.hpp
)

add_executable(sandbox ${MAIN_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MAIN_SOURCES})

set_target_properties(sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../runtime_data"
)

target_link_libraries(sandbox PRIVATE common)

set(DLL_DIR ${CMAKE_SOURCE_DIR}/vendor/DLLs)
file(GLOB DLL_FILES "${DLL_DIR}/*.dll")

foreach(DLL ${DLL_FILES})
    add_custom_command(TARGET sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL}"
            "$<TARGET_FILE_DIR:sandbox>"
    )
endforeach()

target_compile_definitions(sandbox PRIVATE
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)
