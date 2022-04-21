#include "Robot.h"
#include "Logger.h"
#include <thread>
#include <chrono>
#include <syscall.h>
#include <signal.h>
#include "Login.pb.h"

using namespace std;
using namespace Vera;

void signal_pipe(int x)
{

}

int main()
{
    signal(SIGPIPE, SIG_IGN);       //send发送时如果对端关闭了连接，那么会直接关掉整个程序，这个捕获一下SIGPIPE，防止程序被关
    vector<thread> ths;
    for (int i = 0; i < 100; i++)
    {
        ths.emplace_back(thread([](){
            vector<shared_ptr<Robot>> robots;
            for (int j = 0; j < 10; j++)
            {
                std::shared_ptr<Robot> robot = std::make_shared<Robot>("127.0.0.1", 8888);
                robots.push_back(robot);
            }
            while(true)
            {
                for (auto& robot : robots)
                {
                    LoginReq req;
                    req.set_account("zhj");
                    req.set_passward(to_string(syscall(SYS_gettid)));
                    robot->NotifyAll(1, req.SerializeAsString());
                    robot->Update();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            } 
        }));
    }
    for (auto& th : ths)
    {
        if (th.joinable())
        {
            th.join();
            LOG_INFO("th join");
        }
    }
    // std::shared_ptr<Robot> robot = std::make_shared<Robot>("127.0.0.1", 8888);
    // robot->Start();
    // while(true)
    // {
    //     LoginReq req;
    //     req.set_account("zhj");
    //     req.set_passward("123");
    //     robot->NotifyAll(0, req.SerializeAsString());
    //     robot->Update();
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // }
    return 0;
}