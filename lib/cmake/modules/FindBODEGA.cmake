# Find Bodega - library to access the Bodega store
#
# This module defines
#  BODEGA_FOUND - whether the qsjon library was found
#  BODEGA_LIBRARIES - the bodega library
#  BODEGA_INCLUDE_DIR - the include path of the bodega library
#

if (BODEGA_INCLUDE_DIR AND BODEGA_LIBRARIES)

  # Already in cache
  set (BODEGA_FOUND TRUE)

else (BODEGA_INCLUDE_DIR AND BODEGA_LIBRARIES)

  if (NOT WIN32)
    # use pkg-config to get the values of BODEGA_INCLUDE_DIRS
    # and BODEGA_LIBRARY_DIRS to add as hints to the find commands.
    include (FindPkgConfig)
    pkg_check_modules (BODEGA Bodega>=0.1)
  endif (NOT WIN32)

  find_library (BODEGA_LIBRARIES
    NAMES
    bodega
    PATHS
    ${BODEGA_LIBRARY_DIRS}
    ${LIB_INSTALL_DIR}
    ${KDE4_LIB_DIR}
  )

  find_path (BODEGA_INCLUDE_DIR
    NAMES
    session.h
    PATH_SUFFIXES
    bodega
    PATHS
    ${BODEGA_INCLUDE_DIRS}
    ${INCLUDE_INSTALL_DIR}
    ${KDE4_INCLUDE_DIR}
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(BODEGA DEFAULT_MSG BODEGA_LIBRARIES BODEGA_INCLUDE_DIR)

endif (BODEGA_INCLUDE_DIR AND BODEGA_LIBRARIES)
