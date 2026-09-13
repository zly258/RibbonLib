cmake_minimum_required(VERSION 3.16)
project(RibbonLibConsumerSmoke LANGUAGES CXX)

find_package(RibbonLib CONFIG REQUIRED)

add_executable(RibbonLibConsumerSmoke
    main.cpp
)

target_link_libraries(RibbonLibConsumerSmoke
    PRIVATE RibbonLib::RibbonLib
)

target_compile_features(RibbonLibConsumerSmoke PRIVATE cxx_std_17)
