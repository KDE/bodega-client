install(FILES
        assetjob.h
        assetoperations.h
        bodegamodel.h
        changelanguagejob.h
        channelsjob.h
        error.h
        globals.h
        imagelocator.h
        networkjob.h
        participantinfojob.h
        session.h
        searchjob.h
        signonjob.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/bodega COMPONENT Devel
)


link_directories(${LINK_DIRECTORIES} ${QJSON_LIBRARY_DIRS})

set (bodega_SRCS
     assetjob.cpp
     assethandler.cpp
     assetoperations.cpp
     bodegamodel.cpp
     changelanguagejob.cpp
     channelsjob.cpp
     error.cpp
     imagelocator.cpp
     installjob.cpp
     networkjob.cpp
     participantinfojob.cpp
     session.cpp
     searchjob.cpp
     signonjob.cpp
     )

include("CMakeQtUsage")

qt4_automoc(${bodega_SRCS})

add_library (bodega SHARED ${bodega_SRCS})
target_link_libraries(bodega  ${QT_LIBRARIES} ${QJSON_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${QJSON_INCLUDE_DIRS})

set_target_properties(bodega PROPERTIES
                      VERSION ${BODEGA_LIB_MAJOR_VERSION}.${BODEGA_LIB_MINOR_VERSION}.${BODEGA_LIB_PATCH_VERSION}
                      SOVERSION ${BODEGA_LIB_MAJOR_VERSION}
                      DEFINE_SYMBOL BODEGA_MAKEDLL
                      )

INSTALL(TARGETS bodega
   LIBRARY DESTINATION ${LIB_INSTALL_DIR}
   RUNTIME DESTINATION bin
   ARCHIVE DESTINATION  ${LIB_INSTALL_DIR}
)

add_subdirectory(handlers)
