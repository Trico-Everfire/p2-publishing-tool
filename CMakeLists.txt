cmake_minimum_required(VERSION 3.15)

project(p2_map_publisher CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "..." FORCE)
endif()

if(UNIX)
	set(QT_INCLUDE "/usr/include/x86_64-linux-gnu/qt5")
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

set(SRC
	# Dialogs
	dialogs/P2MapPublisher.h
	dialogs/P2MapPublisher.cpp
	dialogs/P2MapMainWindow.h
	dialogs/P2MapMainWindow.cpp
	dialogs/P2PublisherAdvancedOptions.h
	dialogs/P2LoopManager.h
	dialogs/P2BSPReader.h
	dialogs/P2PTIDialog.h
	P2Publish.cpp
)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/res/appicon.rc)

if(UNIX)
	set(STEAM_API  "${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/linux64/libsteam_api.so")
elseif(WIN32)
	# MinGW doesn't like this, but the steam api DLL is incompatible with it anyway
	set(STEAM_API  "${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/win64/steam_api64.lib")
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/win64/steam_api64.dll" "${CMAKE_BINARY_DIR}/steam_api64.dll" COPYONLY)
endif()

add_executable(${PROJECT_NAME} ${SRC} res/zoo_textures.qrc ${APP_ICON_RESOURCE_WINDOWS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui ${STEAM_API})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${QT_INCLUDE} ${QT_INCLUDE}/QtWidgets" "${QT_INCLUDE}/QtGui" "${QT_INCLUDE}/QtCore")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/public/steam")

if(WIN32)
	# Copy these to bundle them with the program in releases
	configure_file("${QT_BASEDIR}/bin/Qt5Core.dll"    "${CMAKE_BINARY_DIR}/Qt5Core.dll"    COPYONLY)
	configure_file("${QT_BASEDIR}/bin/Qt5Gui.dll"     "${CMAKE_BINARY_DIR}/Qt5Gui.dll"     COPYONLY)
	configure_file("${QT_BASEDIR}/bin/Qt5Widgets.dll" "${CMAKE_BINARY_DIR}/Qt5Widgets.dll" COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/platforms/qwindows.dll"         "${CMAKE_BINARY_DIR}/platforms/qwindows.dll"         COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/platforms/qwindowsd.dll"        "${CMAKE_BINARY_DIR}/platforms/qwindowsd.dll"        COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/styles/qwindowsvistastyle.dll"  "${CMAKE_BINARY_DIR}/styles/qwindowsvistastyle.dll"  COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/styles/qwindowsvistastyled.dll" "${CMAKE_BINARY_DIR}/styles/qwindowsvistastyled.dll" COPYONLY)
endif()
