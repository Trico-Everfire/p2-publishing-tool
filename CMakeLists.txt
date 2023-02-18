cmake_minimum_required(VERSION 3.15)

project(p2_map_uploader CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MZ_DECOMPRESS_ONLY ON)

# Qt
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "..." FORCE)
endif ()

if (UNIX)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(QT_INCLUDE "$ENV{HOME}/Qt/6.4.1/gcc_64/include/")
endif ()

if (WIN32)
    set(CMAKE_CXX_STANDARD 23)
    # On Windows, the install dir must be specified, as it's not standardized
    if (DEFINED QT_BASEDIR)
        # Add it to the prefix path so find_package can find it

        set(QT_INCLUDE ${QT_BASEDIR}/include)
        # CMake has an odd policy that links a special link lib for Qt on newer versions of CMake. Enable it so we don't get spammed, and I get to write less
        cmake_policy(SET CMP0020 NEW)
    else ()
        message(FATAL_ERROR "--!@ Please define your QT install dir with -DQT_BASEDIR=C:/your/Qt6/here")
    endif ()
endif ()

message("Using ${QT_INCLUDE} as our Qt include dir")
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Network)
list(APPEND LINK_LIBS Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Network keyvalues)
list(APPEND INC_DIRS ${QT_INCLUDE} ${QT_INCLUDE}/QtWidgets ${QT_INCLUDE}/QtGui ${QT_INCLUDE}/QtNetwork ${QT_INCLUDE}/QtCore)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(USE_STEAM "Uses steam" OFF)


# Main Application
set(SRC
        src/main.cpp
        src/mainview.cpp
        src/mainview.h
        src/mapuploader.cpp
        src/mapuploader.h
        src/filedialogpreset.h
        src/bspentityelementparser.cpp
        src/bspentityelementparser.h
        src/bspfilestructure.h)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/SpeedyKeyV")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/SAPP")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/libz-ng-minizip-ng")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/SpeedyKeyV")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/SAPP")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/libz-ng-minizip-ng")
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SRC} res/resource.qrc)

set(STEAM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-API)
add_library(steam_api64 SHARED IMPORTED)
include_directories("${STEAM_DIR}/public/steam")
list(APPEND LINK_DIRS "${STEAM_DIR}/redistributable_bin")
if (WIN32)
    list(APPEND LINK_LIBS win64/steam_api64)
else ()
#    list(APPEND LINK_LIBS ${CMAKE_BINARY_DIR}/libsteam_api.so)
    list(APPEND LINK_LIBS ${STEAM_DIR}/redistributable_bin/linux64/libsteam_api.so)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBS})
target_link_directories(${PROJECT_NAME} PUBLIC ${LINK_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBS})
target_link_directories(${PROJECT_NAME} PUBLIC ${LINK_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIRS})

if (WIN32)
    # Set up the debugger so it can run the program without copying a million dlls
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${QT_BASEDIR}/bin;")

    # Copy the windows binaries.
    configure_file("${QT_BASEDIR}/bin/Qt6Core.dll"    "${CMAKE_BINARY_DIR}/Qt6Core.dll"    COPYONLY)
    configure_file("${QT_BASEDIR}/bin/Qt6Gui.dll"     "${CMAKE_BINARY_DIR}/Qt6Gui.dll"     COPYONLY)
    configure_file("${QT_BASEDIR}/bin/Qt6Widgets.dll" "${CMAKE_BINARY_DIR}/Qt6Widgets.dll" COPYONLY)
    configure_file("${QT_BASEDIR}/bin/Qt6Network.dll" "${CMAKE_BINARY_DIR}/Qt6Network.dll" COPYONLY)
    configure_file("${QT_BASEDIR}/../../Tools/OpenSSL/Win_x64/bin/libcrypto-1_1-x64.dll" "${CMAKE_BINARY_DIR}/libcrypto-1_1-x64.dll" COPYONLY)
    configure_file("${QT_BASEDIR}/../../Tools/OpenSSL/Win_x64/bin/libssl-1_1-x64.dll" "${CMAKE_BINARY_DIR}/libssl-1_1-x64.dll" COPYONLY)
    configure_file("${QT_BASEDIR}/plugins/platforms/qwindows.dll"         "${CMAKE_BINARY_DIR}/platforms/qwindows.dll"         COPYONLY)
    configure_file("${STEAM_DIR}/redistributable_bin/win64/steam_api64.dll" "${CMAKE_BINARY_DIR}/steam_api64.dll" COPYONLY)
endif ()

if (UNIX)
    configure_file("${STEAM_DIR}/redistributable_bin/linux64/libsteam_api.so" "${CMAKE_BINARY_DIR}/libsteam_api.so" COPYONLY)
endif ()

