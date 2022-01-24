#include <condition_variable>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>

using namespace std;

class Account {
public:
  Account(string name, double money): mName(name), mMoney(money) {};

public:
  void changeMoney(double amount) {
    unique_lock lock(mMoneyLock); // 这里使用的是unique_lock，这是为了与条件变量相配合。因为条件变量会解锁和重新锁定互斥体。
    mConditionVar.wait(lock, [this, amount] {
      // 这里是比较重要的一个地方：通过条件变量进行等待。此时：会通过后面的lambda表达式判断条件是否满足。如果满足则继续；如果不满
      // 足，则此处会解锁互斥体，并让当前线程等待。解锁这一点非常重要，因为只有这样，才能让其他线程获取互斥体。
      return mMoney + amount > 0; // 使用lambda表达式，这里是条件变量等待的条件。
    });
    mMoney += amount;
    // 此处也很重要。当金额发生变动之后，我们需要通知所有在条件变量上等待的其他线程。此时所有调用wait线程都会再次唤醒，然后尝试获取
    // 锁（当然，只有一个能获取到）并再次判断条件是否满足。除了notify_all还有notify_one，它只通知一个等待的线程。wait和notify
    // 就构成了线程间互相协作的工具。
    mConditionVar.notify_all(); // 在通过之后，通知所有的wait
  }

  string getName() {
    return mName;
  }

  double getMoney() {
    return mMoney;
  }

private:
  string mName;
  double mMoney;
  mutex mMoneyLock;
  condition_variable mConditionVar; // 这里声明了一个条件变量，用来在多个线程之间协作。
};

class Bank {
public:
  void addAccount(Account* account) {
    mAccounts.insert(account);
  }

  // 有了上面的改动之后，银行的转账方法实现起来就很简单了，不用再考虑数据保护的问题了
  void transferMoney(Account* accountA, Account* accountB, double amount) {
    accountA->changeMoney(-amount);
    accountB->changeMoney(amount);
  }

  double totalMoney() const {
    double sum = 0;
    for (auto a : mAccounts) {
      sum += a->getMoney();
    }
    return sum;
  }

private:
  set<Account*> mAccounts;
};

mutex sCoutLock;
// 转账逻辑也会变得简单，不用再管转账失败的情况发生
void randomTransfer(Bank* bank, Account* accountA, Account* accountB) {
  while(true) {
    double randomMoney = ((double)rand() / RAND_MAX) * 100;
    {
      lock_guard guard(sCoutLock);
      cout << "Try to Transfer " << randomMoney
           << " from " << accountA->getName() << "(" << accountA->getMoney()
           << ") to " << accountB->getName() << "(" << accountB->getMoney()
           << "), Bank totalMoney: " << bank->totalMoney() << endl;
    }
    bank->transferMoney(accountA, accountB, randomMoney);
  }
}

int main() {
  Account a("Paul", 100);
  Account b("Moira", 100);

  Bank aBank;
  aBank.addAccount(&a);
  aBank.addAccount(&b);

  thread t1(randomTransfer, &aBank, &a, &b);
  thread t2(randomTransfer, &aBank, &b, &a);

  t1.join();
  t2.join();

  return 0;
}