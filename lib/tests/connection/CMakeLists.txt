# Use it
INCLUDE( ${QT_USE_FILE} )

INCLUDE(AddFileDependencies)

link_directories(${LINK_DIRECTORIES} ${qjson_LIBRARY_DIRS})

# Include the library include directories, and the current build directory (moc)
INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/libs/bodega
  ${CMAKE_CURRENT_BINARY_DIR}
  ${qjson_INCLUDE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include
)
qt4_wrap_cpp(tester_MOC_SRCS tester.h)

ADD_EXECUTABLE(
  connection
  main.cpp
  tester.cpp
  ${tester_MOC_SRCS}
)

TARGET_LINK_LIBRARIES(
  connection
  ${QT_LIBRARIES}
  ${qjson_LIBRARIES}
  bodega
)
