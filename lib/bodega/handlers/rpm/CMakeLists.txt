
link_directories(${LINK_DIRECTORIES} ${QJSON_LIBRARY_DIRS})

set (rpmhandlerplugin_SRCS
     rpmhandler.cpp
     rpminstalljob.cpp
     rpmuninstalljob.cpp
     packageidinstalljob.cpp
     )

include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bodega ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${PACKAGEKIT_QT2_INCLUDE_DIR})

include("../../CMakeQtUsage")

qt4_automoc(${rpmhandlerplugin_SRCS})

add_library (rpmhandlerplugin SHARED ${rpmhandlerplugin_SRCS})
target_link_libraries(rpmhandlerplugin  ${QT_QTNETWORK_LIBRARY}  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QJSON_LIBRARIES} ${PACKAGEKIT_QT2_LIB} bodega)


INSTALL(TARGETS rpmhandlerplugin
   DESTINATION ${QT_PLUGINS_DIR}/bodegaassethandlers/
)


