cmake_minimum_required(VERSION 3.16)

project(p2_map_publisher CXX)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_INSTALL_RPATH $ORIGIN)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

function(SYMLINK OLD NEW)
	INSTALL(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${OLD} ${NEW} )")
endfunction()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "..." FORCE)
endif()

if(UNIX)
	set(QT_INCLUDE "/usr/include/x86_64-linux-gnu/qt5")
	if (NOT DEFINED QT_BASEDIR)
		set(QT_BASEDIR "$ENV{HOME}/Qt/5.15.2/gcc_64/")
	endif ()
	if (DEFINED QT_BASEDIR)
		set(CMAKE_PREFIX_PATH "${QT_BASEDIR}/lib/cmake")
	else ()
		message(FATAL_ERROR "--!@ Please define your QT install dir with -DQT_BASEDIR=C:/your/qt5/here")
	endif ()
elseif(WIN32)
	# On Windows, the install dir must be specified, as it's not standardized
	if(DEFINED QT_BASEDIR)
		# Add it to the prefix path so find_package can find it
		list(APPEND CMAKE_PREFIX_PATH ${QT_BASEDIR})
		set(QT_INCLUDE ${QT_BASEDIR}/include)
		# CMake has an odd policy that links a special link lib for Qt on newer versions of CMake. Enable it so we don't get spammed, and I get to write less
		cmake_policy(SET CMP0020 NEW)
	else()
		message(FATAL_ERROR "--!@ Please define your QT install dir with -DQT_BASEDIR=C:/your/qt5/here")
	endif()
endif()

message("Using ${QT_INCLUDE} as our Qt include dir")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(P2_MAP_PUBLISHER_HEADERS
		dialogs/P2LoopManager.h
		dialogs/P2BSPReader.h
		dialogs/P2PTIDialog.h
		dialogs/P2PublisherAdvancedOptions.h
		dialogs/P2MapMainWindow.h
		dialogs/P2DialogConfig.h
		dialogs/P2MapPublisher.h
		dialogs/P2ElementParser.h
)

set(P2_MAP_PUBLISHER_SRC
		# Dialogs
		dialogs/P2MapPublisher.cpp
		dialogs/P2MapMainWindow.cpp
		dialogs/P2ElementParser.cpp
		P2Publish.cpp
		)
list(APPEND P2_MAP_PUBLISHER_SRC ${P2_MAP_PUBLISHER_HEADERS})

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Network)

set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/res/appicon.rc)

if(UNIX)
   	configure_file("${QT_BASEDIR}/lib/libQt5Core.so.5.15.2" "${CMAKE_BINARY_DIR}/libQt5Core.so.5" COPYONLY)
	configure_file("${QT_BASEDIR}/lib/libQt5Gui.so.5.15.2" "${CMAKE_BINARY_DIR}/libQt5Gui.so.5" COPYONLY)
	configure_file("${QT_BASEDIR}/lib/libQt5Widgets.so.5.15.2" "${CMAKE_BINARY_DIR}/libQt5Widgets.so.5" COPYONLY)
	configure_file("${QT_BASEDIR}/lib/libQt5Network.so.5.15.2" "${CMAKE_BINARY_DIR}/libQt5Network.so.5" COPYONLY)
	configure_file("${QT_BASEDIR}/lib/libicui18n.so.56" "${CMAKE_BINARY_DIR}/libicui18n.so.56" COPYONLY)
	configure_file("${QT_BASEDIR}/lib/libicuuc.so.56" "${CMAKE_BINARY_DIR}/libicuuc.so.56" COPYONLY)
	configure_file("${QT_BASEDIR}/lib/libicudata.so.56" "${CMAKE_BINARY_DIR}/libicudata.so.56" COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/imageformats/libqtga.so" "${CMAKE_BINARY_DIR}/libqtga.so")
	configure_file("${QT_BASEDIR}/plugins/platforms/libqminimal.so" "${CMAKE_BINARY_DIR}/platforms/libqminimal.so" COPYONLY)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/linux64/libsteam_api.so" "${CMAKE_BINARY_DIR}/libsteam_api.so" COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/platforms/libqxcb.so" "${CMAKE_BINARY_DIR}/platforms/libqxcb.so" COPYONLY)
	set(STEAM_API  "${CMAKE_BINARY_DIR}/libsteam_api.so")
elseif(WIN32)
	# MinGW doesn't like this, but the steam api DLL is incompatible with it anyway
	set(STEAM_API  "${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/win64/steam_api64.lib")
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/win64/steam_api64.dll" "${CMAKE_BINARY_DIR}/steam_api64.dll" COPYONLY)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Credits.txt" "${CMAKE_BINARY_DIR}/Credits.txt" COPYONLY)

add_subdirectory( libs/SpeedyKeyV )

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources)

add_executable(${PROJECT_NAME} ${P2_MAP_PUBLISHER_SRC} res/zoo_textures.qrc libs/theme/breeze.qrc ${APP_ICON_RESOURCE_WINDOWS} ${link_target})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${P2_MAP_PUBLISHER_HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network ${STEAM_API} keyvalues)
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin")

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${QT_INCLUDE} ${QT_INCLUDE}/QtWidgets" "${QT_INCLUDE}/QtGui" "${QT_INCLUDE}/QtCore" "${QT_INCLUDE}/QtNetwork")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/public/steam")
#include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/public/steam")


if(WIN32)
	# Copy these to bundle them with the program in releases
	configure_file("${QT_BASEDIR}/bin/Qt5Core.dll"    "${CMAKE_BINARY_DIR}/Qt5Core.dll"    COPYONLY)
	configure_file("${QT_BASEDIR}/bin/Qt5Gui.dll"     "${CMAKE_BINARY_DIR}/Qt5Gui.dll"     COPYONLY)
	configure_file("${QT_BASEDIR}/bin/Qt5Widgets.dll" "${CMAKE_BINARY_DIR}/Qt5Widgets.dll" COPYONLY)
	configure_file("${QT_BASEDIR}/bin/Qt5Network.dll" "${CMAKE_BINARY_DIR}/Qt5Network.dll" COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/platforms/qwindows.dll"         "${CMAKE_BINARY_DIR}/platforms/qwindows.dll"         COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/platforms/qwindowsd.dll"        "${CMAKE_BINARY_DIR}/platforms/qwindowsd.dll"        COPYONLY)
endif()