cmake_minimum_required(VERSION 3.16)
project(RibbonLib VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(RIBBONLIB_BUILD_EXAMPLE_DEFAULT ON)
    set(RIBBONLIB_ENABLE_INSTALL_DEFAULT ON)
else()
    set(RIBBONLIB_BUILD_EXAMPLE_DEFAULT OFF)
    set(RIBBONLIB_ENABLE_INSTALL_DEFAULT OFF)
endif()

option(RIBBONLIB_BUILD_EXAMPLE "Build RibbonLib example" ${RIBBONLIB_BUILD_EXAMPLE_DEFAULT})
option(RIBBONLIB_BUILD_SHARED "Build RibbonLib as shared library" OFF)
option(RIBBONLIB_ENABLE_INSTALL "Enable RibbonLib install targets" ${RIBBONLIB_ENABLE_INSTALL_DEFAULT})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(RIBBON_PUBLIC_HEADERS
    src/RibbonLib.h
    src/RibbonLibGlobal.h
    src/QRibbonButtonSize.h
    src/QRibbonWidget.h
    src/QRibbonTab.h
    src/QRibbonGroup.h
    src/QRibbonButton.h
    src/QApplicationButton.h
    src/QRibbonHelper.h
    src/QRibbonMenu.h
    src/QRibbonSplitButton.h
)

set(RIBBON_PRIVATE_HEADERS
    src/QRibbonMetrics.h
    src/QRibbonStyle.h
    src/QRibbonTextLayout.h
)

set(RIBBON_SOURCES
    src/QRibbonStyle.cpp
    src/QRibbonWidget.cpp
    src/QRibbonTab.cpp
    src/QRibbonGroup.cpp
    src/QRibbonButton.cpp
    src/QApplicationButton.cpp
    src/QRibbonHelper.cpp
    src/QRibbonMenu.cpp
    src/QRibbonSplitButton.cpp
)

set(RIBBONLIB_LIBRARY_TYPE STATIC)
if(RIBBONLIB_BUILD_SHARED)
    set(RIBBONLIB_LIBRARY_TYPE SHARED)
endif()

add_library(RibbonLib ${RIBBONLIB_LIBRARY_TYPE}
    ${RIBBON_SOURCES}
    ${RIBBON_PUBLIC_HEADERS}
    ${RIBBON_PRIVATE_HEADERS}
    resource/ribbonlib.qrc
)
add_library(RibbonLib::RibbonLib ALIAS RibbonLib)

target_include_directories(RibbonLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/RibbonLib>
)

target_link_libraries(RibbonLib PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
target_compile_features(RibbonLib PUBLIC cxx_std_17)

if(RIBBONLIB_BUILD_SHARED)
    target_compile_definitions(RibbonLib PRIVATE RIBBONLIB_BUILD_LIBRARY)
else()
    target_compile_definitions(RibbonLib PUBLIC RIBBONLIB_STATIC)
endif()

if(MSVC)
    target_compile_options(RibbonLib PRIVATE /utf-8 /MP /W3 /nologo)
else()
    target_compile_options(RibbonLib PRIVATE -Wall -Wextra)
endif()

set_target_properties(RibbonLib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(RIBBONLIB_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(RIBBONLIB_ENABLE_INSTALL)
    install(TARGETS RibbonLib
        EXPORT RibbonLibTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RibbonLib
    )

    install(FILES
        ${RIBBON_PUBLIC_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RibbonLib
    )

    install(FILES
        LICENSE
        README.md
        README.zh-CN.md
        docs/USAGE.md
        docs/USAGE.zh-CN.md
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RibbonLibConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/RibbonLibConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RibbonLib"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/RibbonLibConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(EXPORT RibbonLibTargets
        FILE RibbonLibTargets.cmake
        NAMESPACE RibbonLib::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RibbonLib"
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/RibbonLibConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/RibbonLibConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RibbonLib"
    )
endif()
