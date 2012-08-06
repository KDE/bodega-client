
link_directories(${LINK_DIRECTORIES} ${QJSON_LIBRARY_DIRS})

find_package(KDE4 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bodega ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${KDE4_INCLUDES} )

include("../../CMakeQtUsage")

find_package(Nepomuk)
find_package(Soprano)

set (bookhandlerplugin_SRCS
     bookhandler.cpp
     bookinstalljob.cpp
     bookuninstalljob.cpp
     )


qt4_automoc(${bookhandlerplugin_SRCS})

add_library (epubhandlerplugin SHARED ${bookhandlerplugin_SRCS})
target_link_libraries(epubhandlerplugin  ${QT_QTNETWORK_LIBRARY}  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QJSON_LIBRARIES} bodega)

if (NEPOMUK_FOUND)
    ADD_DEFINITIONS(-DUSE_NEPOMUK)
    target_link_libraries(epubhandlerplugin ${NEPOMUK_LIBRARIES})
endif (NEPOMUK_FOUND)

INSTALL(TARGETS epubhandlerplugin
   DESTINATION ${QT_PLUGINS_DIR}/bodegaassethandlers/
)


