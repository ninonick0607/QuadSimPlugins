include(CMakeFindDependencyMacro)

if(NOT ON)

    if(NOT OFF)
        find_dependency(CURL REQUIRED CONFIG)
    endif()

    find_dependency(Threads REQUIRED)
    find_dependency(jsoncpp REQUIRED)
    find_dependency(tinyxml2 REQUIRED)
    find_dependency(LibLZMA REQUIRED)
    find_dependency(LibEvents REQUIRED)

    if(OFF)
        find_dependency(gRPC)
    endif()
endif()

get_filename_component(MAVSDK_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include("${MAVSDK_CMAKE_DIR}/MAVSDKTargets.cmake")
