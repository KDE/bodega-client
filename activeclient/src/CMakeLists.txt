
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} ${BODEGA_INCLUDE_DIR} common)

set(active-addons_SRCS
    main.cpp
    bodegastore.cpp

    common/dataenginebindings.cpp
    common/kdeclarativeview.cpp
    common/kdeclarativemainwindow.cpp
)

set(bodegaupdater_xml ../../kded_updater/org.kde.BodegaUpdater.xml)

QT4_ADD_DBUS_INTERFACE(active-addons_SRCS ${bodegaupdater_xml} bodegaupdater_interface)

KDE4_ADD_EXECUTABLE(active-addons ${active-addons_SRCS})

target_link_libraries(active-addons
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${KDE4_PLASMA_LIBS}
    kdeclarative
    bodega
)

install(TARGETS active-addons ${INSTALL_TARGETS_DEFAULT_ARGS} )

