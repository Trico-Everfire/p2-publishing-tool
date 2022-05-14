if(UNIX)
	configure_file("${PROJECT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/linux64/libsteam_api.so" "${CMAKE_BINARY_DIR}/libsteam_api.so" COPYONLY)
	set(STEAM_API  "${CMAKE_BINARY_DIR}/libsteam_api.so")
elseif(WIN32)
	# MinGW doesn't like this, but the steam api DLL is incompatible with it anyway
	set(STEAM_API  "${PROJECT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/win64/steam_api64.lib")
	configure_file("${PROJECT_SOURCE_DIR}/libs/Steam-Workshop-API/sdk/redistributable_bin/win64/steam_api64.dll" "${CMAKE_BINARY_DIR}/steam_api64.dll" COPYONLY)
endif()
