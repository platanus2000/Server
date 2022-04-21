# 设置库子目录
set(SUB_LIB_DIR_LIST "common" "util" "interface" "network" "network/buffer"
        "network/protobuf" "network/protobuf" "thread" "zinx" "mysql"
        )

# 收集source文件
# file(GLOB_RECURSE HEAD_FILES *.h)
file(GLOB_RECURSE LIB_FILES *.cpp *.cc *.hpp)

add_library(${MY_LIB_NAME} STATIC ${LIB_FILES})
message("LIB_FILES: ${LIB_FILES}")

target_include_directories(${MY_LIB_NAME} PUBLIC ${SUB_LIB_DIR_LIST})
# target_include_directories(${MY_LIB_NAME} PUBLIC /usr/local/include/google)

target_link_libraries(${MY_LIB_NAME} -lpthread -lprotobuf)

# message("Source files: ${LIB_FILES}")
# message("Head files: ${HEAD_FILES}")
message("SUB_LIB_DIR_LIST: ${SUB_LIB_DIR_LIST}")