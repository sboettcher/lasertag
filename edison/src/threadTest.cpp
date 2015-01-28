#include <unistd.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <future>
#include <vector>

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

void test(int x) {
  printf("START %i\n", x);
  fflush(stdout);
  usleep(1000000*x);
  printf("END %i\n", x);
  fflush(stdout);
}

void thread(int x) {
  std::vector<std::future<void>> handles;
  for (int i = 0; i < x; i++) {
    handles.push_back(std::async(std::launch::async, test, i));
  }
  for (auto& h : handles) h.get();
}


int main() {
  //std::thread second(bar, 20);
  //std::thread first(foo, 10);

  //first.join();
  //second.join();

  std::thread testing(thread, 20);

  testing.join();

  return 0;
}
