set(LIBS_DIR "../libs")
CollectIncludeDirectories(${LIBS_DIR} APPS_INCLUDE_DIRECTORIES)
message("APPS_INCLUDE_DIRECTORIES: ${APPS_INCLUDE_DIRECTORIES}")
include_directories(${APPS_INCLUDE_DIRECTORIES} ${DIR_LUA}/include)

add_subdirectory(server)
add_subdirectory(robot)
add_subdirectory(zinxapp)
add_subdirectory(utiltest)