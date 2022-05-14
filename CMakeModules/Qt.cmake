set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Network PATHS ${PROJECT_SOURCE_DIR}/dependecies/qt5/qtbase/lib/cmake/Qt5)

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

if(UNIX)
    configure_file("${PROJECT_SOURCE_DIR}/dependecies/qt5/qtbase/lib/libQt5Core.so.5.15.2" "${CMAKE_BINARY_DIR}/libQt5Core.so.5" COPYONLY)
	configure_file("${PROJECT_SOURCE_DIR}/dependecies/qt5/qtbase/lib/libQt5Gui.so.5.15.2" "${CMAKE_BINARY_DIR}/libQt5Gui.so.5" COPYONLY)
	configure_file("${PROJECT_SOURCE_DIR}/dependecies/qt5/qtbase/lib/libQt5Widgets.so.5.15.2" "${CMAKE_BINARY_DIR}/libQt5Widgets.so.5" COPYONLY)
	configure_file("${PROJECT_SOURCE_DIR}/dependecies/qt5/qtbase/lib/libQt5Network.so.5.15.2" "${CMAKE_BINARY_DIR}/libQt5Network.so.5" COPYONLY)
elseif(WIN32)
	# Copy these to bundle them with the program in releases
	configure_file("${QT_BASEDIR}/bin/Qt5Core.dll"    "${CMAKE_BINARY_DIR}/Qt5Core.dll"    COPYONLY)
	configure_file("${QT_BASEDIR}/bin/Qt5Gui.dll"     "${CMAKE_BINARY_DIR}/Qt5Gui.dll"     COPYONLY)
	configure_file("${QT_BASEDIR}/bin/Qt5Widgets.dll" "${CMAKE_BINARY_DIR}/Qt5Widgets.dll" COPYONLY)
	configure_file("${QT_BASEDIR}/bin/Qt5Network.dll" "${CMAKE_BINARY_DIR}/Qt5Network.dll" COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/platforms/qwindows.dll"         "${CMAKE_BINARY_DIR}/platforms/qwindows.dll"         COPYONLY)
	configure_file("${QT_BASEDIR}/plugins/platforms/qwindowsd.dll"        "${CMAKE_BINARY_DIR}/platforms/qwindowsd.dll"        COPYONLY)
endif()
