file(GLOB_RECURSE MAIN_SOURCES *.cpp *.c *.h *.hpp)

add_executable(main ${MAIN_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MAIN_SOURCES})

target_link_libraries(main PRIVATE common)

set_target_properties(main PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../runtime_data"
)

# Copy dll after build
file(GLOB DLL_FILES "${CMAKE_SOURCE_DIR}/vendor/DLLs/*.dll")
foreach(DLL ${DLL_FILES})
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL}" "$<TARGET_FILE_DIR:main>"
    )
endforeach()