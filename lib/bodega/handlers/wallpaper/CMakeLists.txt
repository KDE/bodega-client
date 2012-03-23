
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

link_directories(${LINK_DIRECTORIES} ${KDE4_INCLUDES} ${QT_INCLUDES} )

set (wallpaperhandlerplugin_SRCS
     wallpaperhandler.cpp
     wallpaperinstalljob.cpp
     )

include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bodega ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${KDE4_INCLUDES})

include("../../CMakeQtUsage")

qt4_automoc(${wallpaperhandlerplugin_SRCS})

add_library (wallpaperhandlerplugin SHARED ${wallpaperhandlerplugin_SRCS})
target_link_libraries(wallpaperhandlerplugin  ${QT_LIBRARIES} ${QJSON_LIBRARIES} ${KDE4_PLASMA_LIBS} bodega)


INSTALL(TARGETS wallpaperhandlerplugin
   DESTINATION ${QT_PLUGINS_DIR}/bodegaassethandler/
)


