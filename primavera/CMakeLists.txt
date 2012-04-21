find_package(KDE4 REQUIRED)

include(KDE4Defaults)

add_subdirectory(src)

# QML-only thing
install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/packages/com.coherenttheory.primavera)
install(FILES vivaldi-primavera.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

