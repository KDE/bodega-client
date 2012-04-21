include_directories(${KDE4_INCLUDES} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/activeclient/src/common)

set(primavera_SRCS
    main.cpp
    ${CMAKE_SOURCE_DIR}/activeclient/src/common/dataenginebindings.cpp
    ${CMAKE_SOURCE_DIR}/activeclient/src/common/kdeclarativeview.cpp
    ${CMAKE_SOURCE_DIR}/activeclient/src/common/kdeclarativemainwindow.cpp
)

KDE4_ADD_EXECUTABLE(primavera ${primavera_SRCS})

target_link_libraries(primavera
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${KDE4_PLASMA_LIBS}
    kdeclarative
)

install(TARGETS primavera ${INSTALL_TARGETS_DEFAULT_ARGS} )

