#include "CASQueue.h"
#include "LockFreeQueue.h"

using namespace std;
int main()
{
    uint64 count = 200000;
    uint16 threadCount = 50;
    auto start = std::chrono::system_clock::now().time_since_epoch();
    LockFreeLinkedQueue<int32> queue;
    vector<thread> ths;
    for (uint16 i = 0; i < threadCount; i++)
    {
        ths.push_back(
        thread([&count, &queue](){
            for (size_t i = 0; i < count; i++)
            {
                queue.Push(i);
            }
            while (!queue.Empty())
            {
                int32 idx = 0;
                queue.TryPop(idx);
            }
        }));
    }
    for (auto& th : ths)
    {
        th.join();
    }
    auto end = std::chrono::system_clock::now().time_since_epoch();
    cout << "item num: " << count << ", spend time: " << chrono::duration_cast<chrono::microseconds>(end - start).count() /1000<< "ms," << " queue size: " << queue.Size() << endl;
    return 0;
}