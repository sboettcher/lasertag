#include <iostream>
#include <thread>
#include <mutex>
#include <string>

std::mutex _mtx;


void printstuff(std::string s, int i) {
  std::cout << s << ": " << i << std::endl;
}

void foo(int x) {
  std::lock_guard<std::mutex> lock(_mtx);
  for(int i = 0; i < x; i++)
    printstuff("foo", i);
}

void bar(int x) {
  std::lock_guard<std::mutex> lock(_mtx);
  for(int i = 0; i < x; i++)
    printstuff("bar", i);
}

int main() {
  std::thread second(bar, 20);
  std::thread first(foo, 10);

  first.join();
  second.join();

  return 0;
}
