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

double concurrent_task(int min, int max) {
  vector<future<double>> results; // 首先创建一个集合来储存future对象。我们将用它来获取任务结果

  unsigned concurrent_count = thread::hardware_concurrency();
  min = 0;
  for (int i = 0; i < concurrent_count; i++) { // 根据CPU的数量来创建线程的数量
    // 将任务包装成packaged_task。请注意，由于concurrent_worker被包装成了任务，我们
    // 无法直接获取它的return值。而是要通过future对象来获取。
    packaged_task<double(int, int)> task(concurrent_worker);
    // 获取任务关联的future对象，并将其存入集合中。
    results.push_back(task.get_future());

    int range = max / concurrent_count * (i + 1);
    // 通过一个新的线程来执行任务，并传入需要的参数。
    thread t(std::move(task), min, range);
    t.detach();

    min = range + 1;
  }

  cout << "threads create finish" << endl;
  double sum = 0;
  for (auto& r : results) {
    // 通过future集合，逐个获取每个任务的计算结果，将其
    // 累加。这里r.get()获取到的就是每个任务中concurrent_worker的返回值。
    sum += r.get();
  }
  return sum;
}

int main() {
  auto start_time = chrono::steady_clock::now();

  double r = concurrent_task(0, MAX);

  auto end_time = chrono::steady_clock::now();
  auto ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
  cout << "Concurrent task finish, " << ms << " ms consumed, Result: " << r << endl;
  return 0;
}