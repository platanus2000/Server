#include <iostream>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
using namespace std;

static void stackDump(lua_State* L)
{
    static int count = 0;
    printf("begin dump lua stack %d\n",count);
    int i = 0;
    int top = lua_gettop(L);
    for (i = top; i >= 1; --i)
    {
        int t = lua_type(L,i);
        switch (t)
        {
            case LUA_TSTRING:
                printf("'%s' ",lua_tostring(L,i));
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(L,i) ? "true " : "false ");
                break;
            case LUA_TNUMBER:
                printf("%g ",lua_tonumber(L,i));
                break;
            default:
                printf("%s ",lua_typename(L,t));
                break;
        }
    }
    printf("\nend dump lua stack %d\n",count++);
}

int main(int argc, char* argv[])
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luaL_dofile(L, "test.lua");

    lua_getglobal(L, "teacher");
    lua_getglobal(L, "price");

    if (lua_isnumber(L, -1))
    {
        cout << lua_tonumber(L, -1) << endl;
    }
    lua_pop(L, 1);
    if (lua_isstring(L, -1))
    {
        const char* str = lua_tostring(L, -1);
        cout << str << endl;
    }
    lua_pop(L, 1);
    lua_getglobal(L, "tab");
//    lua_pushstring(L, "teacher");
//    lua_gettable(L, -2);
    stackDump(L);
    cout << "stack size: " << lua_gettop(L) << endl;
    lua_getfield(L, -1, "teacher");     //等价于上面注释的两句
    if (lua_isstring(L, -1))
    {
        const char* str = lua_tostring(L, -1);
        cout << str << endl;
    }
    lua_pop(L, 1);

    lua_pushstring(L, "price");
    lua_gettable(L, -2);
    if (lua_isnumber(L, -1))
    {
        cout << lua_tonumber(L, -1) << endl;
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "func");
    lua_pcall(L, 0, 3, 0);
    cout << "res: " << lua_tointeger(L, -1) << endl;
    cout << "res: " << lua_tointeger(L, -2) << endl;
    cout << "res: " << lua_tointeger(L, -3) << endl;

    lua_pop(L, -1);

    cout << "==== lua func ====" << endl;
    lua_getglobal(L, "func");
    lua_pushnumber(L, 101);
    lua_pushstring(L, "zhj");
    lua_newtable(L);
    lua_pushstring(L, "A");
    lua_pushstring(L, "c++传递表作为参数");
    lua_settable(L, 4);
    lua_pcall(L, 3, 3, 0);
    cout << "stack size: " << lua_gettop(L) << endl;
    cout << "res: " << lua_tointeger(L, -1) << endl;
    cout << "res: " << lua_tointeger(L, -2) << endl;
    cout << "res: " << lua_tointeger(L, -3) << endl;
    lua_pop(L, -1);

    lua_close(L);

    return 0;
}
