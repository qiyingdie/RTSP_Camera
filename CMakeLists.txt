cmake_minimum_required(VERSION 3.19)
project(CameraTest LANGUAGES CXX)

# 自动处理 .ui / .h / .qrc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Multimedia
    MultimediaWidgets
    Gui
)

qt_standard_project_setup()

qt_add_executable(CameraTest
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    SprbScan021.png
)

qt_add_resources(CameraTest "my_resources"
    PREFIX "/myimage"
    FILES
        SprbScan021.png
)

target_link_libraries(CameraTest
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::Gui
)

include(GNUInstallDirs)

install(TARGETS CameraTest
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CameraTest
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
