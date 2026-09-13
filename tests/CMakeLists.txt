add_executable(RibbonLibSmoke
    smoke.cpp
)

target_link_libraries(RibbonLibSmoke
    PRIVATE
    RibbonLib::RibbonLib
    Qt${QT_VERSION_MAJOR}::Widgets
)

add_test(NAME RibbonLibSmoke COMMAND RibbonLibSmoke)
set_tests_properties(RibbonLibSmoke PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
