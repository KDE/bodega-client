
find_package(QPackageKit REQUIRED)

link_directories(${LINK_DIRECTORIES} ${QJSON_LIBRARY_DIRS})

set (rpmhandlerplugin_SRCS
     rpmhandler.cpp
     rpminstalljob.cpp
     rpmuninstalljob.cpp
     )

include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bodega ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${QPACKAGEKIT_INCLUDE_DIR})

include("../../CMakeQtUsage")

qt4_automoc(${rpmhandlerplugin_SRCS})

add_library (rpmhandlerplugin SHARED ${rpmhandlerplugin_SRCS})
target_link_libraries(rpmhandlerplugin  ${QT_LIBRARIES} ${QJSON_LIBRARIES} ${QPACKAGEKIT_LIB} bodega)


INSTALL(TARGETS rpmhandlerplugin
   DESTINATION ${QT_PLUGINS_DIR}/bodegaassethandlers/
)


