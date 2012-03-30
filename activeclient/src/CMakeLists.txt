
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} ${BODEGA_INCLUDE_DIR} common)

set(bodega-store_SRCS
    main.cpp
    bodegastore.cpp

    common/dataenginebindings.cpp
    common/kdeclarativeview.cpp
    common/kdeclarativemainwindow.cpp
)

KDE4_ADD_EXECUTABLE(bodega-store ${bodega-store_SRCS})

target_link_libraries(bodega-store
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${KDE4_PLASMA_LIBS}
    kdeclarative
    bodega
)

install(TARGETS bodega-store ${INSTALL_TARGETS_DEFAULT_ARGS} )

