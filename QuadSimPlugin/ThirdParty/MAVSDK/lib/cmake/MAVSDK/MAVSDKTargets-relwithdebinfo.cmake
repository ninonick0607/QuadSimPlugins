#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "MAVSDK::mavsdk" for configuration "RelWithDebInfo"
set_property(TARGET MAVSDK::mavsdk APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(MAVSDK::mavsdk PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/mavsdk.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/mavsdk.dll"
  )

list(APPEND _cmake_import_check_targets MAVSDK::mavsdk )
list(APPEND _cmake_import_check_files_for_MAVSDK::mavsdk "${_IMPORT_PREFIX}/lib/mavsdk.lib" "${_IMPORT_PREFIX}/bin/mavsdk.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
