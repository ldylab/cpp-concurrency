#include <iostream>
#include <thread> // 为了使用多线程的接口，需要包含#include<thread>头文件

using namespace std;

void hello() { // 新建线程的入口是一个普通的函数，并没有什么特别的地方
  cout << "Hello World from new thread." << endl;
}

int main() {
  // 创建线程的方式是构造一个thread对象，并指定入口函数，与普通的对象不一样的是，此时编译器会为我们创建
  // 一个新的操作系统线程，并在新的线程中执行我们的入口函数
  thread t(hello);
  t.join();

  return 0;
}