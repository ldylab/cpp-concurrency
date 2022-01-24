#include <cmath>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

static const int MAX = 10e8;

double concurrent_worker(int min, int max) {
  double sum = 0;
  for (int i = min; i <= max; i++) {
    sum += sqrt(i);
  }
  return sum;
}

void concurrent_task(int min, int max, promise<double>* result) {
  // concurrent_task不再直接返回计算结果，而是增加了一个promise对象来存放结果。
  vector<future<double>> results;

  unsigned concurrent_count = thread::hardware_concurrency();
  min = 0;
  for (int i = 0; i < concurrent_count; i++) {
    packaged_task<double(int, int)> task(concurrent_worker);
    results.push_back(task.get_future());

    int range = max / concurrent_count * (i + 1);
    thread t(std::move(task), min, range);
    t.detach();

    min = range + 1;
  }

  cout << "threads create finish" << endl;
  double sum = 0;
  for (auto& r : results) {
    sum += r.get();
  }
  // 在任务计算完成之后，将总结过设置到promise对象上。一旦这里调用了set_value，其相关联的future对象就会就绪。
  result->set_value(sum);
  cout << "concurrent_task finish" << endl;
}

int main() {
  auto start_time = chrono::steady_clock::now();

  // 这里是在main中创建一个promoise来存放结果，并以指针的形式传递进concurrent_task中。
  promise<double> sum;
  concurrent_task(0, MAX, &sum);

  auto end_time = chrono::steady_clock::now();
  auto ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
  cout << "Concurrent task finish, " << ms << " ms consumed." << endl;
  // 通过sum.get_future().get()来获取结果。第2点中已经说了：一旦调用了set_value，其相关联的future对象就会就绪。
  cout << "Result: " << sum.get_future().get() << endl;
  return 0;
}