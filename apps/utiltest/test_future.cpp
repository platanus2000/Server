#include <future>
#include <string>
#include <thread>
#include <iostream>

using namespace std;

int add(int a, int b)
{
    return a+b;
}

void print(std::promise<std::string>& p)
{
	p.set_value("There is the result whitch you want.");
}

void do_some_other_things()
{
	std::cout << "Hello World" << std::endl;
}

int main()
{
    promise<std::string> ps;
    thread th(&print, std::ref(ps));
    future<std::string> ft = ps.get_future();
    do_some_other_things();
    while (!ft.valid());
    cout << ft.get() << endl;
    th.join();

    packaged_task<int(int, int)> task(add);
    future<int> ft1 = task.get_future();
    do_some_other_things();
    task(1, 2);
    cout << ft1.get() << endl;
    return 0;
}
