cmake_minimum_required(VERSION 3.16)

add_executable(RibbonExample
    main.cpp
    MainWindow.cpp
    MainWindow.h
    RibbonJsonWindow.cpp
    ExampleRibbonAction.cpp
    RibbonJsonWindow.h
    ExampleRibbonAction.h
)

target_link_libraries(RibbonExample
    RibbonLib
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(RibbonExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

foreach(_cfg Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${_cfg}" _cfg_upper)
    set_target_properties(RibbonExample PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${_cfg_upper} "${CMAKE_CURRENT_BINARY_DIR}/bin"
        PDB_OUTPUT_DIRECTORY_${_cfg_upper} "${CMAKE_CURRENT_BINARY_DIR}/bin"
    )
endforeach()

target_include_directories(RibbonExample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

if(MSVC)
    target_compile_options(RibbonExample PRIVATE /MP /utf-8 /W3 /nologo)
else()
    target_compile_options(RibbonExample PRIVATE -Wall -Wextra)
endif()

add_custom_command(TARGET RibbonExample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            ${CMAKE_CURRENT_BINARY_DIR}/bin/example/resources
    COMMENT "Copy RibbonExample resources to example output directory"
)
