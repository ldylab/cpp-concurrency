#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

void generateRandomData(vector<double>& collection, int size) {
  random_device rd;
  mt19937 mt(rd());
  uniform_real_distribution<double> dist(1.0, 100.0);
  for (int i = 0; i < size; i++) {
    collection.push_back(dist(mt));
  }
}

int main() {
  vector<double> collection;
  generateRandomData(collection, 10e6); // 通过一个函数生成1000,000个随机数。

  // 将数据拷贝3份，以备使用。
  vector<double> copy1(collection);
  vector<double> copy2(collection);
  vector<double> copy3(collection);

  // 接下来将通过三个不同的parallel_policy参数来调用同样的sort算法。每次调用记录开始和结束的时间。
  auto time1 = chrono::steady_clock::now();
  // 第一次调用使用std::execution::seq参数。
  sort(execution::seq, copy1.begin(), copy1.end());
  auto time2 = chrono::steady_clock::now();
  auto duration = chrono::duration_cast<chrono::milliseconds>(time2 - time1).count();
  // 输出本次测试所使用的时间。
  cout << "Sequenced sort consuming " << duration << "ms." << endl;

  auto time3 = chrono::steady_clock::now();
  // 第二次调用使用std::execution::par参数。
  sort(execution::par, copy2.begin(),copy2.end());
  auto time4 = chrono::steady_clock::now();
  duration = chrono::duration_cast<chrono::milliseconds>(time4 - time3).count();
  cout << "Parallel sort consuming " << duration << "ms." << endl;

  auto time5 = chrono::steady_clock::now();
  // 第三次调用使用std::execution::par_unseq参数。
  sort(execution::par_unseq, copy2.begin(),copy2.end());
  auto time6 = chrono::steady_clock::now();
  duration = chrono::duration_cast<chrono::milliseconds>(time6 - time5).count();
  cout << "Parallel unsequenced sort consuming " << duration << "ms." << endl;
}

/* 结果输出
Sequenced sort consuming 4464ms.
Parallel sort consuming 459ms.
Parallel unsequenced sort consuming 168ms.
可以看到，性能最好的和最差的相差了超过26倍。
*/