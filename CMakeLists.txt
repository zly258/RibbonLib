cmake_minimum_required(VERSION 3.16)
project(RibbonLib LANGUAGES CXX)

set(CMAKE_RULE_MESSAGES OFF CACHE BOOL "" FORCE)
set(CMAKE_VERBOSE_MAKEFILE OFF CACHE BOOL "" FORCE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(RIBBONLIB_BUILD_EXAMPLE "Build RibbonLib example" ON)
option(RIBBONLIB_BUILD_SHARED "Build RibbonLib as shared library" OFF)
option(RIBBONLIB_ENABLE_INSTALL "Enable RibbonLib install targets" OFF)

set(RIBBONLIB_PUBLISH_ROOT "${CMAKE_BINARY_DIR}/publish" CACHE PATH "RibbonLib publish root")
set(RIBBONLIB_BIN_DIR "${RIBBONLIB_PUBLISH_ROOT}/bin")
set(RIBBONLIB_LIB_DIR "${RIBBONLIB_PUBLISH_ROOT}/lib")
set(RIBBONLIB_INC_DIR "${RIBBONLIB_PUBLISH_ROOT}/inc")
set(RIBBONLIB_RESOURCE_DIR "${RIBBONLIB_PUBLISH_ROOT}/resource")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)

set(RIBBON_CORE_HEADERS
    src/RibbonLibGlobal.h
    src/QRibbonButtonSize.h
    src/QRibbonMetrics.h
    src/QRibbonWidget.h
    src/QRibbonTab.h
    src/QRibbonGroup.h
    src/QRibbonButton.h
    src/QApplicationButton.h
    src/RibbonAction.h
)

set(RIBBON_CORE_SOURCES
    src/QRibbonWidget.cpp
    src/QRibbonTab.cpp
    src/QRibbonGroup.cpp
    src/QRibbonButton.cpp
    src/QApplicationButton.cpp
    src/RibbonAction.cpp
)

set(RIBBON_EXTENDED_HEADERS
    src/QRibbonHelper.h
    src/QRibbonMenu.h
    src/QRibbonSplitButton.h
    src/QRibbonStateManager.h
)

set(RIBBON_EXTENDED_SOURCES
    src/QRibbonHelper.cpp
    src/QRibbonMenu.cpp
    src/QRibbonSplitButton.cpp
    src/QRibbonStateManager.cpp
)

if(RIBBONLIB_BUILD_SHARED)
    set(RIBBONLIB_LIBRARY_TYPE SHARED)
else()
    set(RIBBONLIB_LIBRARY_TYPE STATIC)
endif()

add_library(RibbonLib ${RIBBONLIB_LIBRARY_TYPE}
    ${RIBBON_CORE_SOURCES}
    ${RIBBON_CORE_HEADERS}
    ${RIBBON_EXTENDED_SOURCES}
    ${RIBBON_EXTENDED_HEADERS}
)

add_library(RibbonLib::RibbonLib ALIAS RibbonLib)

target_include_directories(RibbonLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(RibbonLib PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
target_compile_features(RibbonLib PUBLIC cxx_std_17)

if(RIBBONLIB_BUILD_SHARED)
    target_compile_definitions(RibbonLib PRIVATE RIBBONLIB_BUILD_LIBRARY)
else()
    target_compile_definitions(RibbonLib PUBLIC RIBBONLIB_STATIC)
endif()

if(MSVC)
    target_compile_options(RibbonLib PUBLIC /utf-8 PRIVATE /MP /W3 /nologo)
else()
    target_compile_options(RibbonLib PRIVATE -Wall -Wextra)
endif()

set_target_properties(RibbonLib PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RIBBONLIB_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${RIBBONLIB_BIN_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${RIBBONLIB_LIB_DIR}"
    PDB_OUTPUT_DIRECTORY "${RIBBONLIB_BIN_DIR}"
)

foreach(_cfg Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${_cfg}" _cfg_upper)
    set_target_properties(RibbonLib PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${_cfg_upper} "${RIBBONLIB_BIN_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_${_cfg_upper} "${RIBBONLIB_BIN_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_${_cfg_upper} "${RIBBONLIB_LIB_DIR}"
        PDB_OUTPUT_DIRECTORY_${_cfg_upper} "${RIBBONLIB_BIN_DIR}"
    )
endforeach()

set(RIBBON_PUBLIC_HEADERS
    ${RIBBON_CORE_HEADERS}
    ${RIBBON_EXTENDED_HEADERS}
)

set(RIBBON_PUBLIC_HEADER_FILES)
foreach(_header ${RIBBON_PUBLIC_HEADERS})
    if(IS_ABSOLUTE "${_header}")
        list(APPEND RIBBON_PUBLIC_HEADER_FILES "${_header}")
    else()
        list(APPEND RIBBON_PUBLIC_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${_header}")
    endif()
endforeach()

add_custom_target(RibbonLibPublish ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RIBBONLIB_INC_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RIBBONLIB_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RIBBONLIB_RESOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${RIBBON_PUBLIC_HEADER_FILES}
            "${RIBBONLIB_INC_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/resource/styles/ribbon.qss"
            "${RIBBONLIB_RESOURCE_DIR}/ribbon.qss"
    COMMENT "Publish RibbonLib to build/publish/inc lib resource"
)

add_dependencies(RibbonLibPublish RibbonLib)

if(RIBBONLIB_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(RIBBONLIB_ENABLE_INSTALL)
    install(TARGETS RibbonLib
        EXPORT RibbonLibTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )

    install(FILES
        ${RIBBON_PUBLIC_HEADER_FILES}
        DESTINATION include
    )

    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/resource/styles/ribbon.qss"
        DESTINATION resource
    )

    install(EXPORT RibbonLibTargets
        NAMESPACE RibbonLib::
        DESTINATION lib/cmake/RibbonLib
    )
endif()
