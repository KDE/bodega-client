find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/lib)

set(kded_bodegaupdater_SRCS updater.cpp)

QT4_ADD_DBUS_ADAPTOR(kded_bodegaupdater_SRCS  org.kde.BodegaUpdater.xml
                     updater.h Updater)

kde4_add_plugin(kded_bodegaupdater ${kded_bodegaupdater_SRCS} )

target_link_libraries(
   kded_bodegaupdater
   ${KDE4_KDECORE_LIBS}
   ${KDE4_KDEUI_LIBS}
   ${KDE4_SOLID_LIBS}
   ${QT_QTSQL_LIBRARY}
   bodega
)

install(TARGETS kded_bodegaupdater DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES bodegaupdater.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kded)

