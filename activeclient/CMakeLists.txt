find_package(KDE4 REQUIRED)
find_package(KActivities REQUIRED)

include(KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(src)

# QML-only thing
install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/packages/com.coherenttheory.bodegastore)
install(FILES bodega-store.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

