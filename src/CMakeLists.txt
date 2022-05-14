include(Qt)
include(ICU)
include(Steam)

set(SRC
	# Dialogs
	dialogs/P2DialogConfig.h
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

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_executable(${PROJECT_NAME} ${SRC} ${PROJECT_SOURCE_DIR}/res/zoo_textures.qrc ${PROJECT_SOURCE_DIR}/libs/theme/breeze.qrc ${APP_ICON_RESOURCE_WINDOWS} ${link_target})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network ${STEAM_API})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${QT_INCLUDE} ${QT_INCLUDE}/QtWidgets" "${QT_INCLUDE}/QtGui" "${QT_INCLUDE}/QtCore" "${QT_INCLUDE}/QtNetwork")
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/public/steam")
