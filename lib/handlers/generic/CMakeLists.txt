
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

link_directories(${LINK_DIRECTORIES} ${QT_INCLUDES} )

set (generichandlerplugin_SRCS
     filethread.cpp
     generichandler.cpp
     genericinstalljob.cpp
     genericuninstalljob.cpp
     )

include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bodega ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES})

SET(QT_DONT_USE_QTGUI TRUE)

qt4_automoc(${generichandlerplugin_SRCS})

add_library (generichandlerplugin SHARED ${generichandlerplugin_SRCS})
target_link_libraries(generichandlerplugin  ${QT_LIBRARIES} ${QJSON_LIBRARIES} bodega)


INSTALL(TARGETS generichandlerplugin
   DESTINATION ${QT_PLUGINS_DIR}/bodegaassethandlers/
)


