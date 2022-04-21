/*
 * Lua调用C++
 * */
#include <iostream>
#include <assert.h>
extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
using namespace std;

int CTest(lua_State* L)
{
    cout << "In CTest" << endl;
    if (lua_isstring(L, 1))
    {
        cout << lua_tostring(L, 1) << endl;
    }
    if (lua_isnumber(L, 2))
    {
        cout << lua_tointeger(L, 2) << endl;
    }
    lua_pop(L, -1);
    return 0;
}

//数组
int CTestArr(lua_State* L)
{
    cout << "In CTestArr" << endl;
    int len = luaL_len(L, 1);
    for (int i = 1; i <= len; i++)
    {
        lua_pushnumber(L ,i);
        lua_gettable(L, 1); //pop index, push table[i]
        cout << lua_tointeger(L, -1) << endl;
        lua_pop(L, 1);
    }
    cout << "stack size: " << lua_gettop(L) << endl;
    return 0;
}

//key-value表
int CTestHash(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushnil(L);
    while (lua_next(L, 1) != 0)
    {
        cout << "key: " << lua_tostring(L, -2);
        cout << "   value: " << lua_tostring(L, -1) << endl;
        lua_pop(L, 1);
    }
    return 0;
}

int CTestRes(lua_State* L)
{
    //lua_pushstring(L, "return value");

    //返回表格
    lua_newtable(L);
    lua_pushstring(L, "teacher");  //key
    lua_pushstring(L, "zhj");   //value
    lua_settable(L, 1);
    lua_pushstring(L, "age");  //key
    lua_pushinteger(L, 33);   //value
    lua_settable(L, 1);
    return 1;
}

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_register(L, "CTest", CTest);
    lua_register(L, "CTestArr", CTestArr);
    lua_register(L, "CTestHash", CTestHash);
    lua_register(L, "CTestRes", CTestRes);
    lua_register(L, "CTestPtr", CTestPtr);
    luaL_loadfile(L, "test3.lua");

    if (lua_pcall(L, 0, 0, 0))
    {
        cout << lua_tostring(L, -1) << endl;
        lua_pop(L, 1);
    }

    lua_close(L);
    return 0;
}