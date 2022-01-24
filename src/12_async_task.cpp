#include <cmath>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

using namespace std;

static const int MAX = 10e8;
static double sum = 0;

void worker(int min, int max) {
  for (int i = min; i <= max; i++) {
    sum += sqrt(i);
  }
}

class Worker {
public:
  Worker(int min, int max): mMin(min), mMax(max) {} // 这里通过一个类来描述任务。这个类是对前面提到的任务的封装。它包含了任务的输入参数，和输出结果。
  double work() { // work函数是任务的主体逻辑。
    mResult = 0;
    for (int i = mMin; i <= mMax; i++) {
      mResult += sqrt(i);
    }
    return mResult;
  }
  double getResult() {
    return mResult;
  }

private:
  int mMin;
  int mMax;
  double mResult;
};

int main() {
  sum = 0;
  // 这里以异步的方式启动了任务。它会返回一个future对象。future用来存储异步任务
  // 的执行结果，关于future我们在后面packaged_task的例子中再详细说明。在这个例
  // 子中我们仅仅用它来等待任务执行完成。
  auto f1 = async(worker, 0, MAX);
  cout << "Async task triggered" << endl;
  // 此处是等待异步任务执行完成。
  f1.wait();
  cout << "Async task finish, result: " << sum << endl << endl;

  double result = 0;
  cout << "Async task with lambda triggered, thread: " << this_thread::get_id() << endl;
  // async：运行新线程，以异步执行任务。
  // deferred：调用方线程上第一次请求其结果时才执行任务，即惰性求值。
  // 在上面这段代码中，我们使用一个lambda表达式来编写异步任务的逻辑，
  // 并通过launch::async明确指定要通过独立的线程来执行任务，同时我们打印出了线程的id。
  auto f2 = async(launch::async, [&result]() {
    // 除了通过函数来指定异步任务，还可以lambda表达式的方式来指定。
    cout << "Lambda task in thread: " << this_thread::get_id() << endl;
    for (int i = 0; i <= MAX; i++) {
      result += sqrt(i);
    }
  });
  f2.wait();
  cout << "Async task with lambda finish, result: " << result << endl << endl;

  Worker w(0, MAX);
  cout << "Task in class triggered" << endl;
  // 通过async执行任务：这里指定了具体的任务函数以及相应的对象。
  // 请注意这里是&w，因此传递的是对象的指针。如果不写&将传入w对象的临时复制。
  auto f3 = async(&Worker::work, &w); 
  f3.wait();
  cout << "Task in class finish, result: " << w.getResult() << endl << endl;

  return 0;
}