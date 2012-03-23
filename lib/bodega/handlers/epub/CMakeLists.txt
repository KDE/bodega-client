

link_directories(${LINK_DIRECTORIES} ${QJSON_LIBRARY_DIRS})

set (bookhandlerplugin_SRCS
     bookhandler.cpp
     bookinstalljob.cpp
     )

include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bodega ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES})

include("../../CMakeQtUsage")

qt4_automoc(${bookhandlerplugin_SRCS})

add_library (epubhandlerplugin SHARED ${bookhandlerplugin_SRCS})
target_link_libraries(epubhandlerplugin  ${QT_LIBRARIES} ${QJSON_LIBRARIES} bodega)


INSTALL(TARGETS epubhandlerplugin
   DESTINATION ${QT_PLUGINS_DIR}/bodegaassethandler/
)


