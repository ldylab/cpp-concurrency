#include <iostream>
#include <thread>

using namespace std;

int main() {
  // thread可以和callable类型一起工作，如果熟悉lambda表达式，可以直接用它来写线程的逻辑
  thread t([] {
    cout << "Hello World from lambda thread." << endl;
  });

  t.join();

  return 0;
}