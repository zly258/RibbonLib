add_executable(RibbonExample main.cpp)

target_link_libraries(RibbonExample PRIVATE RibbonLib::RibbonLib)
target_compile_features(RibbonExample PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(RibbonExample PRIVATE /utf-8 /MP /W3 /nologo)
else()
    target_compile_options(RibbonExample PRIVATE -Wall -Wextra)
endif()
