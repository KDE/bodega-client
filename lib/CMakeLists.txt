PROJECT(bodega)

set(BODEGA_LIB_MAJOR_VERSION "0")
set(BODEGA_LIB_MINOR_VERSION "1")
set(BODEGA_LIB_PATCH_VERSION "0")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII" )

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules" "${CMAKE_INSTALL_PREFIX}/share/apps/cmake/modules")

set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})

# compile in debug mode
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF("${CMAKE_BUILD_TYPE}" MATCHES "^Rel.*")
  ADD_DEFINITIONS("-DQT_NO_DEBUG_OUTPUT")
  ADD_DEFINITIONS("-DQT_NO_DEBUG_OUTPUT")
ENDIF("${CMAKE_BUILD_TYPE}" MATCHES "^Rel.*")

# Ability to disable verbose debug output
IF(BODEGA_VERBOSE_DEBUG_OUTPUT)
  ADD_DEFINITIONS("-DBODEGA_VERBOSE_DEBUG_OUTPUT")
endif(BODEGA_VERBOSE_DEBUG_OUTPUT)

# Find the dependencies
FIND_PACKAGE( Qt4 COMPONENTS QtCore QtNetwork QtSql REQUIRED )
FIND_PACKAGE( QJSON REQUIRED )

#add extra search paths for libraries and includes
SET (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
SET (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Directory where lib will install")
SET (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")
SET (CMAKE_MODULES_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/apps/cmake/modules" CACHE PATH "The directory to install FindQRpc.cmake to")

set(BODEGA_LIB_VERSION_STRING "${BODEGA_LIB_MAJOR_VERSION}.${BODEGA_LIB_MINOR_VERSION}.${BODEGA_LIB_PATCH_VERSION}")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib${LIB_SUFFIX}" )

# pkg-config
IF (NOT WIN32)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/bodega.pc.in
                  ${CMAKE_CURRENT_BINARY_DIR}/bodega.pc
                  @ONLY)
  INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/bodega.pc
           DESTINATION lib${LIB_SUFFIX}/pkgconfig)
ENDIF (NOT WIN32)

# Include the cmake file needed to use qt4
INCLUDE( ${QT_USE_FILE} )

# Subdirs
add_subdirectory(bodega)
IF (KDE4_BUILD_TESTS OR BODEGA_BUILD_TESTS)
  enable_testing()
  ADD_SUBDIRECTORY(tests)
ENDIF (KDE4_BUILD_TESTS OR BODEGA_BUILD_TESTS)

add_subdirectory(cmake/modules)


#CONFIGURE_FILE(
#  "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
#  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
#  IMMEDIATE @ONLY)

#ADD_CUSTOM_TARGET(uninstall
#  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
