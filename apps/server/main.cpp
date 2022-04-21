#include "Server.h"
#include "ThreadMgr.h"
#include "MsgHandler.h"
int main()
{
    for (int i = 0; i < 3; i++)
    {
        Singleton<ThreadMgr>::Instance()->NewThread();
    }
    Singleton<ThreadMgr>::Instance()->StartAllThread();
    std::unique_ptr<IPacketDispatcher> msgHandler(new MsgHandler());
    std::shared_ptr<Server> server = std::make_shared<Server>("0.0.0.0", 8888, msgHandler);

    Singleton<ThreadMgr>::Instance()->AddObjToThread(server);
    bool isRun = true;
    while (isRun)
    {
        isRun = Singleton<ThreadMgr>::Instance()->IsGameLoop();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}