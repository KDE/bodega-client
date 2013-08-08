
set (bodega_HEADERS
     assetjob.h
     assetoperations.h
     assetoperations_p.h
     collectionaddassetjob.h
     collectionlistassetsjob.h
     collectionlistassetsjobmodel.h
     collectionremoveassetjob.h
     bodegamodel.h
     changelanguagejob.h
     channelsjob.h
     createcollectionjob.h
     deletecollectionjob.h
     error.h
     globals.h
     historymodel.h
     imagelocator.h
     installjob.h
     listcollectionsjob.h
     listcollectionsjobmodel.h
     networkjob.h
     participantinfojob.h
     registerjob.h
     session_p.h
     session.h
     signonjob.h
     uninstalljob.h
     ratingattributesjob.h
     assetratingsjob.h
     participantratingsjob.h
     assetratingsjobmodel.h
     ratingattributesjobmodel.h
     participantratingsjobmodel.h
    )

set (bodega_SRCS
     assetjob.cpp
     assethandler.cpp
     assethandlerfactory.cpp
     assetoperations.cpp
     assetoperations_p.cpp
     collectionaddassetjob.cpp
     collectionlistassetsjob.cpp
     collectionlistassetsjobmodel.cpp
     collectionremoveassetjob.cpp
     bodegamodel.cpp
     changelanguagejob.cpp
     channelsjob.cpp
     createcollectionjob.cpp
     deletecollectionjob.cpp
     error.cpp
     historymodel.cpp
     imagelocator.cpp
     installjob.cpp
     installjobsmodel.cpp
     listcollectionsjob.cpp
     listcollectionsjobmodel.cpp
     networkjob.cpp
     participantinfojob.cpp
     registerjob.cpp
     session.cpp
     signonjob.cpp
     uninstalljob.cpp
     ratingattributesjob.cpp
     assetratingsjob.cpp
     participantratingsjob.cpp
     assetratingsjobmodel.cpp
     ratingattributesjobmodel.cpp
     participantratingsjobmodel.cpp
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
