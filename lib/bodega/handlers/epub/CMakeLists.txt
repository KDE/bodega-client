
link_directories(${LINK_DIRECTORIES} ${QJSON_LIBRARY_DIRS})

find_package(KDE4 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/libs/bodega/ ${CMAKE_SOURCE_DIR}/libs/bodega/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/bodega ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${KDE4_INCLUDES} )

include("../../CMakeQtUsage")

find_package(Soprano)
macro_log_feature(
   Soprano_FOUND "Soprano" "Semantic Desktop Storing" "http://soprano.sourceforge.net"
   TRUE "${SOPRANO_MIN_VERSION}" "Soprano is needed for Nepomuk"
   )

find_package(NepomukCore)
macro_log_feature(NepomukCore_FOUND "NepomukCore" "The NepomukCore libraries" "http://www.kde.org" TRUE "" "Nepomuk is used to save the metadata of installed assets")

set (bookhandlerplugin_SRCS
     bookhandler.cpp
     bookinstalljob.cpp
     bookuninstalljob.cpp
     )


qt4_automoc(${bookhandlerplugin_SRCS})

add_library (epubhandlerplugin SHARED ${bookhandlerplugin_SRCS})
target_link_libraries(epubhandlerplugin  ${QT_QTNETWORK_LIBRARY}  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QJSON_LIBRARIES} bodega)

if (NepomukCore_FOUND)
    ADD_DEFINITIONS(-DUSE_NEPOMUK)

    soprano_add_ontology(bookhandlerplugin_SRCS "${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nfo.trig" "NFO" "Nepomuk2::Vocabulary" "trig")
    soprano_add_ontology(bookhandlerplugin_SRCS "${SHAREDDESKTOPONTOLOGIES_ROOT_DIR}/nie/nco.trig" "NCO" "Nepomuk2::Vocabulary" "trig")


    target_link_libraries(epubhandlerplugin ${NEPOMUK_CORE_LIBRARY})
endif (NEPOMUKCORE_FOUND)

INSTALL(TARGETS epubhandlerplugin
   DESTINATION ${QT_PLUGINS_DIR}/bodegaassethandlers/
)


