extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <luasocket.h>
}
#include <iostream>
using namespace std;

//打印栈
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
	lua_State* L = luaL_newstate();     //lua 上下文 状态机
    luaL_openlibs(L);                   //打开所有库

    lua_pushinteger(L, 1);
    lua_pushinteger(L, 2);
    lua_pushinteger(L, 3);
    stackDump(L);

/* int lua_gettop(lua_State *L)
* 返回栈顶元素的索引。
* 因为栈中元素的索引是从 1 开始编号的，所以函数的返回值相当于栈中元素的个数。
* 返回值为 0 表示栈为空。
*/
/* void lua_settop(lua_State *L，int index)
* 设置栈顶为索引"index"指向处。
* 如果"index"比"lua_gettop()"的值大，那么多出的新元素将被赋值为"nil"。
*/
/* void lua_pushvalue (lua_State *L，int index);
* 将索引"index"处元素，压到栈顶。
*/
/* void lua_replace(lua_State *L，int index)
* 将栈顶元素移动到索引"index"处。(相当于覆盖了索引"index"处的元素)
*/
/* void lua_rotate (lua_State *L，int index，int n);
* 将从"index"处元素到栈顶，参与旋转如 n 为正，向栈顶方向，若为负向栈底方向.
*/
    lua_pop(L, 1);      //-1是清空栈
//    lua_remove(L, 2);
//    lua_insert(L, 2);
    stackDump(L);

    if (luaL_loadfile(L, "main.lua"))
    {
        cout << lua_tostring(L, -1) << endl;
        return -1;
    }
	if (lua_pcall(L, 0, 0, 0))
	{
		cout << lua_tostring(L, -1) << endl;
		return -1;
	}

	lua_close(L);
	return 0;
}