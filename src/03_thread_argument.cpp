#include <iostream>
#include <string>
#include <thread>

using namespace std;

// 可以传递函数给入口函数
// 这个地方要注意的是，参数是以拷贝的形式传递的，因此对于拷贝耗时的对象可能需要传递指针或是引用类型作为参数
// 但是，如果是传递指针或是应用，还需要考虑参数对象的生命周期，因为线程的运行长度很可能会超过参数的生命周期
// 这个时候如果线程还在访问一个已经被销毁的对象就会出现问题。
void hello(string name) {
  cout << "Welcome to " << name << endl;
}

int main() {
  thread t(hello, "https://paul.pub");
  t.join();

  return 0;
}