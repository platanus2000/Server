
IF(UNIX)
    ADD_DEFINITIONS("-O2 -Wall -DLUA_USE_LINUX -DLUA_COMPAT_5_2")
    ADD_DEFINITIONS(-DLUA_COMPAT_MODULE)
ELSEIF(WIN32)
	ADD_DEFINITIONS(-DWIN32)
	ADD_DEFINITIONS(-D_WINDOWS)
    ADD_DEFINITIONS("/WX- /W3")
    ADD_DEFINITIONS(-DLUA_COMPAT_MODULE)
    ADD_DEFINITIONS(-DLUA_COMPAT_APIINTCASTS)
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI /Od /RTC1 /Gm")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /O2")
ENDIF()

set(LUA_SRC_PATH ./)
set(LUA_SOCKET_SRC_PATH ../luasocket)

# lpack begin
set (PACK_SRC ../lpack/lpack.c)
# lpack end

# luasocket begin
set ( LUA_SOCKET_SRC
    ${LUA_SOCKET_SRC_PATH}/auxiliar.c
    ${LUA_SOCKET_SRC_PATH}/buffer.c
    ${LUA_SOCKET_SRC_PATH}/except.c
    ${LUA_SOCKET_SRC_PATH}/inet.c
    ${LUA_SOCKET_SRC_PATH}/io.c
    ${LUA_SOCKET_SRC_PATH}/luasocket.c
    ${LUA_SOCKET_SRC_PATH}/mime.c
    ${LUA_SOCKET_SRC_PATH}/options.c
    ${LUA_SOCKET_SRC_PATH}/select.c
    ${LUA_SOCKET_SRC_PATH}/tcp.c
    ${LUA_SOCKET_SRC_PATH}/timeout.c
    ${LUA_SOCKET_SRC_PATH}/udp.c 
)

set(LUA_CORE_SRC
    ${LUA_SRC_PATH}/lapi.c
    ${LUA_SRC_PATH}/lcode.c
    ${LUA_SRC_PATH}/lctype.c
    ${LUA_SRC_PATH}/ldebug.c
    ${LUA_SRC_PATH}/ldo.c
    ${LUA_SRC_PATH}/ldump.c
    ${LUA_SRC_PATH}/lfunc.c
    ${LUA_SRC_PATH}/lgc.c
    ${LUA_SRC_PATH}/llex.c
    ${LUA_SRC_PATH}/lmem.c
    ${LUA_SRC_PATH}/lobject.c
    ${LUA_SRC_PATH}/lopcodes.c
    ${LUA_SRC_PATH}/lparser.c
    ${LUA_SRC_PATH}/lstate.c
    ${LUA_SRC_PATH}/lstring.c
    ${LUA_SRC_PATH}/ltable.c
    ${LUA_SRC_PATH}/ltm.c
    ${LUA_SRC_PATH}/lundump.c
    ${LUA_SRC_PATH}/lvm.c
    ${LUA_SRC_PATH}/lzio.c
)

set(LUA_LIB_SRC
    ${LUA_SRC_PATH}/lauxlib.c
    ${LUA_SRC_PATH}/lbaselib.c
    ${LUA_SRC_PATH}/lbitlib.c
    ${LUA_SRC_PATH}/lcorolib.c
    ${LUA_SRC_PATH}/ldblib.c
    ${LUA_SRC_PATH}/liolib.c
    ${LUA_SRC_PATH}/lmathlib.c
    ${LUA_SRC_PATH}/loslib.c
    ${LUA_SRC_PATH}/lstrlib.c
    ${LUA_SRC_PATH}/ltablib.c
    ${LUA_SRC_PATH}/lutf8lib.c
    ${LUA_SRC_PATH}/loadlib.c
    ${LUA_SRC_PATH}/linit.c
)

if ( WIN32 AND NOT CYGWIN )
    list (APPEND LUA_SOCKET_SRC ${LUA_SOCKET_SRC_PATH}/wsocket.c)
else ( )
    list (APPEND LUA_SOCKET_SRC ${LUA_SOCKET_SRC_PATH}/usocket.c)
endif ( )

# FILE(GLOB_RECURSE LUA_SOCKET_HEADS ${CMAKE_CURRENT_SOURCE_DIR}/luasocket/*.h)
# luasocket end
set ( LUA_IDSIZE 120 CACHE STRING "gives the maximum size for the description of the source." )
configure_file ( ${LUA_SRC_PATH}/luaconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/luaconf.h )

IF(WIN32)
    add_compile_options(/wd4244)
    add_compile_options(/wd4013)
ENDIF()

# SET_RUNTIME_MT()

# FILE(GLOB_RECURSE SOURCES ${LUA_SRC_PATH}/*.c)
# FILE(GLOB_RECURSE HEADS ${LUA_SRC_PATH}/*.h)
# list (REMOVE_ITEM SOURCES ${LUA_SRC_PATH}/lua.c)

# SOURCE_GROUP(headers FILES ${HEADS})
# SOURCE_GROUP(sources FILES ${SOURCES})
SOURCE_GROUP(core FILES ${LUA_CORE_SRC})
SOURCE_GROUP(lib FILES ${LUA_LIB_SRC})
SOURCE_GROUP(luasocket FILES ${LUA_SOCKET_SRC})
SOURCE_GROUP(lpack FILES ${PACK_SRC})

INCLUDE_DIRECTORIES(
    ${INC_WMGE}
	${LUA_SRC_PATH}    
    ${LUA_SOCKET_SRC_PATH}
    ${CMAKE_CURRENT_BINARY_DIR}
	)

# ADD_LIBRARY(${LIB_LUA} STATIC EXCLUDE_FROM_ALL ${SOURCES} ${HEADS} ${LUA_SOCKET_SRC} ${PACK_SRC})
ADD_LIBRARY(${LIB_LUA} STATIC EXCLUDE_FROM_ALL ${LUA_CORE_SRC} ${LUA_LIB_SRC} ${LUA_SOCKET_SRC} ${PACK_SRC})
SET(LIBRARY_OUTPUT_PATH ${DIR_LUA}/lib)
SET_PROPERTY(TARGET ${LIB_LUA} PROPERTY FOLDER ${LIB_3RD_FOLDER})
