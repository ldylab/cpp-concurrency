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
    mMoney += amount;
  }
  string getName() {
    return mName;
  }
  double getMoney() {
    return mMoney;
  }
  mutex* getLock() {
    return &mMoneyLock;
  }

private:
  string mName;
  double mMoney;
  mutex mMoneyLock;
};

class Bank {
public:
  void addAccount(Account* account) {
    mAccounts.insert(account);
  }

  bool transferMoney(Account* accountA, Account* accountB, double amount) {
    // 在lock_guard中执行

    // 这里通过lock函数来获取两把锁，标准库的实现会保证不会发生死锁。
    
    // lock_guard在下面我们还会详细介绍。这里只要知道它会在自身对象
    // 生命周期的范围内锁定互斥体即可。创建lock_guard的目的是为了在
    // transferMoney结束的时候释放锁，lockB也是一样。但需要注意的
    // 是，这里传递了 adopt_lock表示：现在是已经获取到互斥体了的状
    // 态了，不用再次加锁（如果不加adopt_lock就是二次锁定了）。

    // lock(*accountA->getLock(), *accountB->getLock());
    // lock_guard lockA(*accountA->getLock(), adopt_lock);
    // lock_guard lockB(*accountB->getLock(), adopt_lock);

    scoped_lock lockAll(*accountA->getLock(), *accountB->getLock());

    if (amount > accountA->getMoney()) {
      return false;
    }

    accountA->changeMoney(-amount);
    accountB->changeMoney(amount);
    return true;
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

// 加入一把锁来保护输出逻辑
mutex sCoutLock;
void randomTransfer(Bank* bank, Account* accountA, Account* accountB) {
  while(true) {
    double randomMoney = ((double)rand() / RAND_MAX) * 100;
    if (bank->transferMoney(accountA, accountB, randomMoney)) {
      // 让这个程序把它的话说完再下一个
      sCoutLock.lock();
      cout << "Transfer " << randomMoney << " from " << accountA->getName()
          << " to " << accountB->getName()
          << ", Bank totalMoney: " << bank->totalMoney() << endl;
      sCoutLock.unlock();
    } else {
      sCoutLock.lock();
      cout << "Transfer failed, "
           << accountA->getName() << " has only " << accountA->getMoney() << ", but "
           << randomMoney << " required" << endl;
      sCoutLock.unlock();
    }
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