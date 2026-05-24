find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets)

add_executable(QFluentTests
    tst_multiviewcombobox.cpp
)

target_link_libraries(QFluentTests
    PRIVATE
        QFluent
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
)

add_test(NAME QFluentTests COMMAND QFluentTests)
