project(arkanoidsb)
cmake_minimum_required(VERSION 2.6)

include(FindPkgConfig)
include(CheckLibraryExists)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

message(STATUS "=== ${CMAKE_SYSTEM_NAME} ===")

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    message(STATUS "=== RELEASE MODE ===")
    add_definitions( "-DNDEBUG" )
    add_definitions( "-Wall -O2" )
else()
    message(STATUS "=== DEBUG MODE ===")
    add_definitions( "-DDEBUG" )
    add_definitions( "-Wall -Wextra -pedantic -O0 -g" )
    if( ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASSERTIONS=2" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DEMANGLE_SUPPORT=1" )
    endif()
endif()

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten" )

    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=1 -s USE_SDL_IMAGE=1 -s STB_IMAGE=1" )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --embed-file ../res/arkanoidsb.pak@arkanoidsb.pak --shell-file ../res/index.html" )

else()

    find_package(SDL REQUIRED)
    find_package(SDL_image REQUIRED)
    find_package(SDL_mixer REQUIRED)
    find_package(X11)

    include_directories(
        ${X11_INCLUDE_DIR}
        ${SDL_INCLUDE_DIR}
        ${SDL_IMAGE_INCLUDE_DIRS}
        ${SDL_MIXER_INCLUDE_DIRS}
        )

endif()

set(APP_TITLE           "Arkanoid: Space Ball" CACHE STRING "Application name")
set(APP_VERSION_MAJOR   "1" CACHE STRING "Application major version")
set(APP_VERSION_MINOR   "0" CACHE STRING "Application minor version")
set(APP_VERSION_RELEASE "0" CACHE STRING "Application release version")
set(VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}${APP_VERSION_RELEASE}")
message(STATUS "Version ${VERSION}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp"
    )

file( GLOB_RECURSE APP_SOURCES "src/*.c" "src/*.cpp")

if(APPLE AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")

    file( GLOB_RECURSE APP_SOURCES ${APP_SOURCES} "src/*.m*")
    list( REMOVE_ITEM APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/assets.cpp")

    set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/res/arkanoidsb.icns)
    set(APP_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/arkanoidsb.pak)
    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${APP_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(arkanoidsb MACOSX_BUNDLE ${APP_ICON} ${APP_RESOURCES} ${APP_SOURCES})

    set_target_properties(arkanoidsb PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist.in
        MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
        )

else()

    add_executable(arkanoidsb ${APP_SOURCES})
    set_target_properties(arkanoidsb PROPERTIES SUFFIX ".html")

endif()

target_link_libraries( arkanoidsb
    ${X11_LIBRARIES}
    ${SDL_LIBRARY}
    ${SDL_IMAGE_LIBRARIES}
    ${SDL_MIXER_LIBRARIES}
    )

if(X11_Xinerama_FOUND)
    target_link_libraries( arkanoidsb
        ${X11_Xinerama_LIB}
        )
endif()
