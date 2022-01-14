cmake_minimum_required(VERSION 3.15)

project(p2_map_publisher CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "..." FORCE)
endif()

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	set(QT_INCLUDE "/usr/include/x86_64-linux-gnu/qt5")
endif()

if(WIN32)
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
set(STEAM_API "${CMAKE_CURRENT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/linux64/libsteam_api.so")
endif()

add_executable(p2_map_publisher ${SRC} res/zoo_textures.qrc ${APP_ICON_RESOURCE_WINDOWS})
target_link_libraries(p2_map_publisher PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui ${STEAM_API})

target_include_directories(p2_map_publisher PRIVATE ${QT_INCLUDE} ${QT_INCLUDE}/QtWidgets ${QT_INCLUDE}/QtGui ${QT_INCLUDE}/QtCore)


if(WIN32)
	# Set up the debugger so it can run the program without copying a million dlls
	set_target_properties(p2_map_publisher PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${QT_BASEDIR}/bin;")
endif()
