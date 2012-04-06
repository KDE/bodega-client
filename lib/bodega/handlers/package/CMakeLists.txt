
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

link_directories(${LINK_DIRECTORIES} ${KDE4_INCLUDES} ${QT_INCLUDES} )

set (plasmoidhandlerplugin_SRCS
     packagehandler.cpp
     packageinstalljob.cpp
     packageuninstalljob.cpp
     )

include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bodega ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${KDE4_INCLUDES})

include("../../CMakeQtUsage")

qt4_automoc(${plasmoidhandlerplugin_SRCS})

add_library (plasmoidhandlerplugin SHARED ${plasmoidhandlerplugin_SRCS})
target_link_libraries(plasmoidhandlerplugin  ${QT_LIBRARIES} ${QJSON_LIBRARIES} ${KDE4_PLASMA_LIBS} bodega)


INSTALL(TARGETS plasmoidhandlerplugin
   DESTINATION ${QT_PLUGINS_DIR}/bodegaassethandlers/
)


