#include "GameChannel.h"
#include "msg.pb.h"
#include "GameMsg.h"
#include "AOIWorld.h"
#include "GameRole.h"

using namespace pb;
using namespace std;
int main(int argc, char* argv[])
{
    ZinxKernel::ZinxKernelInit();
    //添加监听通道
    ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8888, new GameConnFact())));
    ZinxKernel::Zinx_Run();
    ZinxKernel::ZinxKernelFini();

    // AOIWorld world(0, 9, 0, 4, 9, 4);
    // world.AddObj(new GridObj(2, 2));
    // world.AddObj(new GridObj(2, 2));
    // world.AddObj(new GridObj(2, 2));
    // world.AddObj(new GridObj(3, 2));
    // world.AddObj(new GridObj(3, 2));
    // world.AddObj(new GridObj(4, 2));

    // auto lis = world.GetSrdObj(new GridObj(3, 1));
    // for (auto obj : lis)
    // {
    //     cout << "gridid: " << world.GetGridID(obj) << endl;
    // }
    return 0;
}