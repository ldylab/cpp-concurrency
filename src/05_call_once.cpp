#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

// 初始化的一个写法（只会执行一次）
void init() {
  cout << "Initialing..." << endl;
  // Do something...
}
// 工程代码
void worker(once_flag* flag) {
  call_once(*flag, init);
}

int main() {
  once_flag flag;

  // 即使有三个线程会使用init函数，但是只会有一个线程真正执行它
  thread t1(worker, &flag);
  thread t2(worker, &flag);
  thread t3(worker, &flag);

  // 使用join的方法
  t1.join();
  t2.join();
  t3.join();

  return 0;
}