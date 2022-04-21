#include "AOIWorld.h"
#include "common.h"
#include <iostream>
using namespace std;

AOIWorld::AOIWorld(int xbegin, int xend, int ybegin, int yend, int xcount, int ycount) :
//为什么写初始化成员列表
x_begin(xbegin), x_end(xend), y_begin(ybegin), y_end(yend), x_count(xcount), y_count(ycount)
{
    x_width = (x_end - x_begin) / x_count;
    y_width = (y_end - y_begin) / y_count;

    for (int i = 0; i < x_count * y_count; i++)
    {
        m_grids.emplace_back(new Grid());
    }
}

AOIWorld::~AOIWorld()
{

}

//获取周围obj
std::list<GridObj*> AOIWorld::GetSrdObj(const Pos& pos)
{
    std::list<GridObj*> objs;
    int grid_id = GetGridID(pos);
    int x_index = grid_id % x_count;
	int y_index = grid_id / x_count;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            uint32 cur_id = (y_index + i) * x_count + x_index + j;
            cout << "cur_id: " << cur_id << endl;
            if (cur_id > 0 && cur_id < m_grids.size())
            {
                list<GridObj*> &cur_list = m_grids[cur_id]->m_gridObjs;
                objs.insert(objs.begin(), cur_list.begin(),cur_list.end());
            }
        }
    }
    return objs;
}

 //添加玩家到AOI网格
bool AOIWorld::AddObj(GridObj* obj)
{
    //计算所属网格编号 从0开始
    int id = GetGridID(obj->GetPos());

    //TO 判断合法性

    //添加
    m_grids[id]->m_gridObjs.push_back(obj);
    cout << "id: " << id << " add" << endl;
    return true;
}

//从网格移除obj
void AOIWorld::DelObj(GridObj* obj)
{
    //计算所属网格编号 从0开始
    int id = GetGridID(obj->GetPos());

    //TO 判断合法性

    //添加
    m_grids[id]->m_gridObjs.remove(obj);
    return;
}

int AOIWorld::GetGridID(const Pos& pos)
{
    return (pos.m_y - y_begin) / y_width * x_count + (pos.m_x - x_begin ) / x_width;
}