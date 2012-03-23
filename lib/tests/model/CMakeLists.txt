# Use it
SET( QT_DONT_USE_QTGUI FALSE )
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
set(model_SRCS
    main.cpp
    modeltest.cpp
    window.cpp)


QT4_WRAP_UI(model_UIS_H window.ui)

ADD_EXECUTABLE(
  model
  ${model_SRCS}
  ${model_UIS_H}
)
qt4_automoc(${model_SRCS})

TARGET_LINK_LIBRARIES(
  model
  ${QT_LIBRARIES}
  ${QT_QTTEST_LIBRARY}
  ${qjson_LIBRARIES}
  bodega
)
