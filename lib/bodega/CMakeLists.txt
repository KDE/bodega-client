SET(QT_DONT_USE_QTGUI TRUE)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTSQL TRUE)

set (bodega_HEADERS
     assetjob.h
     assetbriefsjob.h
     assetoperations.h
     collectionaddassetjob.h
     collectionlistassetsjob.h
     collectionassetsmodel.h
     collectionsmodel.h
     collectionremoveassetjob.h
     credentialsprovider.h
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
     networkjob.h
     participantinfojob.h
     registerjob.h
     session.h
     signonjob.h
     updatescheckjob.h
     updatedassetsmodel.h
     uninstalljob.h
     ratingattributesjob.h
     assetratingsjob.h
     participantratingsjob.h
     assetratingsmodel.h
     participantratingsmodel.h
    )

set (bodega_SRCS
     assetjob.cpp
     assetbriefsjob.cpp
     assethandler.cpp
     assethandlerfactory.cpp
     assetoperations.cpp
     collectionaddassetjob.cpp
     collectionlistassetsjob.cpp
     collectionassetsmodel.cpp
     collectionremoveassetjob.cpp
     collectionsmodel.cpp
     credentialsprovider.cpp
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
     installjobscheduler.cpp
     listcollectionsjob.cpp
     networkjob.cpp
     participantinfojob.cpp
     ratingsmodel_p.cpp
     registerjob.cpp
     session.cpp
     signonjob.cpp
     updatescheckjob.cpp
     updatedassetsmodel.cpp
     uninstalljob.cpp
     ratingattributesjob.cpp
     assetratingsjob.cpp
     participantratingsjob.cpp
     assetratingsmodel.cpp
     participantratingsmodel.cpp
     )

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


