#pragma once
#include <list>
#include <vector>
#include "common.h"

enum ObjType
{
    OBJTYPE_PLAYER       = 1,
    OBJTYPE_CREATURE     = 2,
};

struct Pos
{
    Pos() : m_x(0), m_y(0) { }
    Pos(int x, int y) : m_x(x), m_y(y) { }
    int m_x;
    int m_y;
};

//拥有横纵坐标的对象
class Player;
class GridObj
{
public:
    GridObj() : _pos(0, 0) { }
    GridObj(int x, int y) : _pos(x, y) { }
    virtual int GetX() { return _pos.m_x; };
    virtual int GetY() { return _pos.m_y; };
    const Pos& GetPos() const { return _pos; }
    void SetPos(int x, int y)
    {
        _pos.m_x = x;
        _pos.m_y = y;
    }
    void SetPos(const Pos& pos)
    {
        _pos.m_x = pos.m_x;
        _pos.m_y = pos.m_y;
    }

    ObjType GetObjType() const { return _objType; }
    void SetObjType(ObjType objType) { _objType = objType; }
    
    inline bool IsPlayer() const { return _objType == OBJTYPE_PLAYER; }
    static Player* ToPlayer(GridObj* o) { if (o && o->IsPlayer()) return reinterpret_cast<Player*>(o); else return nullptr; }
    static Player const* ToPlayer(GridObj const* o) { if (o && o->IsPlayer()) return reinterpret_cast<Player const*>(o); else return nullptr; }
    Player* ToPlayer() { return ToPlayer(this); }
    Player const* ToPlayer() const { return ToPlayer(this); }

private:
    Pos _pos;
    ObjType _objType;
    uint32 _objID;
};

//网格 包含若干GridObj的容器
class Grid
{
public:
    std::list<GridObj*>  m_gridObjs;
};

//游戏世界矩形
class AOIWorld
{
public:
    //通过构造函数指定矩形大小和分割粒度
    AOIWorld(int xbegin, int xend, int ybegin, int yend, int xcount, int ycount);
    virtual ~AOIWorld();

    std::vector<Grid*> m_grids;

    //获取周围obj
    std::list<GridObj*> GetSrdObj(const Pos& pos);

    //添加玩家到AOI网格
    bool AddObj(GridObj* obj);

    //从网格移除obj
    void DelObj(GridObj* obj);

    int GetGridID(const Pos& pos);

private:
    //矩形起始结束坐标
    int x_begin = 0;
    int x_end = 0;
    int y_begin = 0;
    int y_end = 0;
    int x_count = 0;
    int y_count = 0;
    int x_width = 0;
    int y_width = 0;
};