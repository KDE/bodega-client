include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/bodega ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT4_INCLUDES} ${QJSON_INCLUDE_DIRS})

find_package(PackageKitQt2)
if (PACKAGEKIT_QT2_FOUND)
   add_subdirectory(rpm)
else (PACKAGEKIT_QT2_FOUND)
   message("Did not find pkgqt2, required for RPM installation handler")
endif (PACKAGEKIT_QT2_FOUND)

add_subdirectory(epub)
add_subdirectory(generic)
add_subdirectory(package)
add_subdirectory(wallpaper)
