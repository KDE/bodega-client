set (bodega_HEADERS
     assetjob.h
     assetoperations.h
     ballotaddassetjob.h
     bodegamodel.h
     changelanguagejob.h
     channelsjob.h
     createballotjob.h
     deleteballotjob.h
     error.h
     globals.h
     imagelocator.h
     installjob.h
     listballotsjob.h
     networkjob.h
     participantinfojob.h
     registerjob.h
     resetpasswordjob.h
     searchjob.h
     session_p.h
     session.h
     signonjob.h
     uninstalljob.h
    )

set (bodega_SRCS
     assetjob.cpp
     assethandler.cpp
     assetoperations.cpp
     ballotaddassetjob.cpp
     bodegamodel.cpp
     changelanguagejob.cpp
     channelsjob.cpp
     createballotjob.cpp
     deleteballotjob.cpp
     error.cpp
     imagelocator.cpp
     installjob.cpp
     listballotsjob.cpp
     networkjob.cpp
     participantinfojob.cpp
     registerjob.cpp
     resetpasswordjob.cpp
     session.cpp
     searchjob.cpp
     signonjob.cpp
     uninstalljob.cpp
     )

include("CMakeQtUsage")

include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${QJSON_INCLUDE_DIRS})

qt4_automoc(${bodega_SRCS})

add_library (bodega SHARED ${bodega_SRCS})
target_link_libraries(bodega  ${QT_LIBRARIES} ${QJSON_LIBRARIES})


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

install(FILES
        ${bodega_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/bodega COMPONENT Devel
)


add_subdirectory(handlers)
