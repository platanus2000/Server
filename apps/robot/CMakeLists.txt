set(APP_NAME vera_robot)

CollectIncludeDirectories(${CMAKE_CURRENT_SOURCE_DIR} SUB_DIR_LIST)
CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_executable(${APP_NAME} ${SRC_FILES})
message("${APP_NAME}'s SUB_DIR_LIST: ${SUB_DIR_LIST}")
target_include_directories(${APP_NAME} PUBLIC ${SUB_DIR_LIST})
add_dependencies(${APP_NAME} ${MY_LIB_NAME})
target_link_libraries(${APP_NAME} ${MY_LIB_NAME} -lpthread -lprotobuf)