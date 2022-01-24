#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

static const int MAX = 10e8;
// 使用一个全局变量来储存结果，并发中会在各个线程中实现对于一个全局变量的修改
static double sum = 0;

// 使用单线程的写法
void worker(int min, int max) {
  for (int i = min; i <= max; i++) {
    sum += sqrt(i);
  }
}

// 计算一个程序所耗费的时间
void serial_task(int min, int max) {
  // 获得当前的时间
  auto start_time = chrono::steady_clock::now();
  sum = 0;
  worker(0, MAX);
  // 获得结束时的时间
  auto end_time = chrono::steady_clock::now();
  // 统计一共耗费了多长的时间
  auto ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
  cout << "Serail task finish, " << ms << " ms consumed, Result: " << sum << endl;
}


// 多线程的写法
void concurrent_task(int min, int max) {
  // 获得了当前的时间
  auto start_time = chrono::steady_clock::now();

  // 获得当前硬件可以支持线程的数据
  unsigned concurrent_count = thread::hardware_concurrency();
  cout << "hardware_concurrency: " << concurrent_count << endl;

  // 其实Thread可以是一种class类型
  vector<thread> threads; 
  min = 0;
  sum = 0;
  for (int t = 0; t < concurrent_count; t++) {
    int range = max / concurrent_count * (t + 1);
    // 对于每一个线程都通过worker函数来完成任务，并划分一部分数据给它处理。
    threads.push_back(thread(worker, min, range));
    min = range + 1;
  }
  // 就像是一种数据类型的使用，在压入vector中时，初始化Thread对象，然后
  // 一点点弹出并使用join执行。
  for (int i = 0; i < threads.size(); i++) {
    // 等待每一个线程执行结束。
    threads[i].join();
  }

  // 获得结束的时候
  auto end_time = chrono::steady_clock::now();
  auto ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
  cout << "Concurrent task finish, " << ms << " ms consumed, Result: " << sum << endl;
}

int main() {
  // 运行「单线程」并获得执行的时间
  serial_task(0, MAX);
  // 运行「多线程」并获得执行的时间
  concurrent_task(0, MAX);
  return 0;
}