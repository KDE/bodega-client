SET( QT_USE_QTTEST TRUE )

# Use it
INCLUDE( ${QT_USE_FILE} )

INCLUDE(AddFileDependencies)

# Include the library include directories, and the current build directory (moc)
INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/
  ${CMAKE_CURRENT_BINARY_DIR}
)
link_directories(${LINK_DIRECTORIES} ${qjson_LIBRARY_DIRS})

add_definitions(-DSOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
set(tst_jobs_SRCS
    dummyserver.cpp
    tst_jobs.cpp)

ADD_EXECUTABLE(
  tst_jobs
  ${tst_jobs_SRCS}
)
qt4_automoc(${tst_jobs_SRCS})

TARGET_LINK_LIBRARIES(
  tst_jobs
  ${QT_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${qjson_LIBRARIES}
  bodega
)

add_test( tst_jobs tst_jobs )
