#include <iostream>
#include <thread>

void foo(int x) {
  for(int i = 0; i < x; i++)
    std::cout << "foo: " << i << std::endl;
}

void bar(int x) {
  for(int i = 0; i < x; i++)
    std::cout << "bar: " << i << std::endl;
}

int main() {
  std::thread first(foo, 10);
  std::thread second(bar, 20);

  first.join();
  second.join();

  return 0;
}
