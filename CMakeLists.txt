cmake_minimum_required(VERSION 3.5)

# Set the Qt installation path
set(CMAKE_PREFIX_PATH "/Users/romanrr/Qt/6.8.0/macos")

project(Focused VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES Focused_en_US.ts)

# List all files explicitly
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    privileges.h
    privileges.cpp
    macos_privileges.h
    macos_privileges.mm
    macos_helpers.h
    macos_helpers.mm

    editor_dialog.h
    editor_dialog.cpp
    editor_dialog.ui

    quadratic_dialog.h
    quadratic_dialog.cpp
    quadratic_dialog.ui

    get_tray_icon_position.h
    get_tray_icon_position.mm

    resources.qrc
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Focused
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        editor_dialog.ui
        quadratic_dialog.ui
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(Focused
        ${PROJECT_SOURCES}
    )
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Focused PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Link the Security framework on macOS
if(APPLE)
    target_link_libraries(Focused PRIVATE "-framework Security")

    set(APP_ICON_MACOS "${CMAKE_SOURCE_DIR}/public/icon.png")
    set_source_files_properties(${APP_ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set(MACOSX_BUNDLE_ICON_FILE "icon.png")
    set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist")
endif()

# Ensure data file is copied to the build directory
add_custom_command(
    TARGET Focused POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/data/distractors.txt
        ${CMAKE_BINARY_DIR}/data/distractors.txt
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Focused)
endif()
set_target_properties(Focused PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Focused
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${CMAKE_SOURCE_DIR}/data/distractors.txt DESTINATION ${CMAKE_INSTALL_DATADIR}/Focused/data)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Focused)
endif()
