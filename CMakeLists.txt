
cmake_minimum_required(VERSION 3.16)
project(Vera)
set(PROJECT_NAME Vera)

set(MY_LIB_NAME vera_lib)

# 设置公共库名字
SET(LIB_JSON	json)
SET(LIB_PROTOBUF protobuf)

# cmake可选项
option(BUILD_LIB3RD "building 3lib if true" ON)
option(USE_REDIS "use redis" ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "-Wall -g -o2 -fstack-protector-all -std=c++17 -DEPOLL")
# add_compile_options(-Wall -g -o2 -fstack-protector-all -std=c++17 -DEPOLL)
# Debug模式下才启用
IF (CMAKE_BUILD_TYPE STREQUAL Debug)
	set(CMAKE_CXX_FLAGS "-fsanitize=address -fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
ENDIF()

# 加载.cmake文件
list(APPEND CMAKE_MODULE_PATH
		"${CMAKE_SOURCE_DIR}/cmake")
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
include(AutoCollect)

# 第三方库include路径
SET(3PARTY_PATH_DIR "${PROJECT_SOURCE_DIR}/lib3rd")
SET(DIR_JSONCPP	${3PARTY_PATH_DIR}/jsoncpp-1.9.4)
SET(DIR_PROTOBUF ${3PARTY_PATH_DIR}/protobuf-3.5.1)
SET(DIR_LUA ${3PARTY_PATH_DIR}/lua-5.3.5)

# json库和protobuf库名
SET(LIB_JSON	 jsoncpp)
SET(LIB_PROTOBUF protobuf)
SET(LIB_LUA lua)

# 优先查找机器上的三方库
find_package(JsonCpp)
if (JsonCpp_FOUND)
    set(3PARTY_PATH_INCLUDE "${JsonCpp_INCLUDE_DIRS} ${3PARTY_PATH_INCLUDE}")
    message("find JsonCpp success")
endif(JsonCpp_FOUND)

find_package(Protobuf)
if (Protobuf_FOUND)
    set(3PARTY_PATH_INCLUDE "${Protobuf_INCLUDE_DIRS} ${3PARTY_PATH_INCLUDE}")
	message("find Protobuf success")
endif(Protobuf_FOUND)

# 如果本地有库就不使用第三方库
if (JsonCpp_FOUND AND Protobuf_FOUND)
    SET(BUILD_LIB3RD OFF)
endif(JsonCpp_FOUND AND Protobuf_FOUND)

# 没找到机器上的库,就自编译第三方库
if (BUILD_LIB3RD)
	SET(3PARTY_PATH_INCLUDE 
		${DIR_PROTOBUF}/src
		${DIR_JSONCPP}/include
		${DIR_LUA}/include
	)

	SET(3PARTY_PATH_LIB 
		${DIR_PROTOBUF}/lib
		${DIR_JSONCPP}/lib
		${DIR_LUA}/lib
	)
	message("ADD_SUBDIRECTORY: ${DIR_JSONCPP}/src/lib_json ${DIR_PROTOBUF}/src")
	ADD_SUBDIRECTORY(${DIR_JSONCPP}/src/lib_json build/jsoncpp)
	ADD_SUBDIRECTORY(${DIR_PROTOBUF}/src build/protobuf)
endif(BUILD_LIB3RD)

# 一定要使用机器上的mysql
find_package(MySQL)
if (MySQL_FOUND)
	INCLUDE_DIRECTORIES(${MySQL_INCLUDE_DIRS})
	LINK_LIBRARIES(${MySQL_LIBRARIES})
	MESSAGE("MySQL_LIBRARIES: ${MySQL_LIBRARIES}")
endif(MySQL_FOUND)

# redis
# if (USE_REDIS)
# 	INCLUDE_DIRECTORIES(/usr/local/include/hiredis)
# 	LINK_LIBRARIES(-lhiredis)
# endif(USE_REDIS)

# 必定自编译第三方库
SET(3PARTY_PATH_INCLUDE 
	${DIR_LUA}/src
)

SET(3PARTY_PATH_LIB 
	${DIR_LUA}/lib
)
ADD_SUBDIRECTORY(${DIR_LUA}/src build/lua)

INCLUDE_DIRECTORIES(${3PARTY_PATH_INCLUDE})

LINK_DIRECTORIES(${3PARTY_PATH_LIB})
LINK_LIBRARIES(${LIB_JSON} ${LIB_PROTOBUF} ${LIB_LUA} -ldl)

add_subdirectory(libs)
add_subdirectory(apps)