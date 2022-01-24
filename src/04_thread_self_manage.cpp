#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

void print_time() {
  // 获取当前的时间
  auto now = chrono::system_clock::now();
  auto in_time_t = chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");

  // 打印当前的时间
  cout << "now is: " << ss.str() << endl;
}

void sleep_thread() {
  // 线程的延时，就是相当于是线程的delay
  this_thread::sleep_for(chrono::seconds(3));
  // 获取当前的thread-id
  cout << "[thread-" << this_thread::get_id() << "] is waking up" << endl;
}

void loop_thread() {
  for (int i = 0; i < 10; i++) {
    // 循环打印当前的thread
    cout << "[thread-" << this_thread::get_id() << "] print: " << i << endl;
  }
}

int main() {
  print_time();

  // 把
  thread t1(sleep_thread);
  thread t2(loop_thread);

  // 等待线程完成执行
  t1.join();
  // 运行线程独立执行
  t2.detach();

  print_time();
  return 0;
}

/* 运行的结果
now is: 2019-10-13 10:17:48
[thread-0x70000cdda000] print: 0
[thread-0x70000cdda000] print: 1
[thread-0x70000cdda000] print: 2
[thread-0x70000cdda000] print: 3
[thread-0x70000cdda000] print: 4
[thread-0x70000cdda000] print: 5
[thread-0x70000cdda000] print: 6
[thread-0x70000cdda000] print: 7
[thread-0x70000cdda000] print: 8
[thread-0x70000cdda000] print: 9
[thread-0x70000cd57000] is waking up
now is: 2019-10-13 10:17:51

这段代码应该还是比较容易理解的，这里创建了两个线程。它们都会有一些输出，
其中一个会先停止3秒钟，然后再输出。主线程调用join会一直卡住等待它运行结束。
*/