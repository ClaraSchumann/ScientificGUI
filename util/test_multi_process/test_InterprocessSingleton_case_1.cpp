#include <cstdlib>
#include <iostream>

#include "InterprocessSingleton.hpp"
#include "InterprocessAccessRole.h"

using namespace boost::interprocess;

struct Test {
  int num;
  char id[128];
};

std::ostream &operator<<(std::ostream &out, const Test &t) {
  std::cout << t.num << std::endl;
  std::cout << t.id << std::endl;

  return out;
};

int main(int argc, char **argv) {
  std::string test_shm_object_name = "test_ip_object";
  if (argc == 1) {
    Test &t = InterprocessSingleton<Test>::instance(
        InterprocessAccessRole::Owner, test_shm_object_name.c_str());

    t.num = 1;
    std::strcpy(t.id, "Dog");

    std::string command = std::string(argv[0]) + " dummy";
    int return_val = std::system(command.c_str());
    (void) return_val;

    std::cout << "\nFrom master process.\n" << t;
  } else {
    Test &t = InterprocessSingleton<Test>::instance(
        InterprocessAccessRole::Guest, test_shm_object_name.c_str());

    std::cout << "\nFrom child process.\n" << t;
    t.num = 2;
    std::strcpy(t.id, "Cat");
    return 0;
  };


  return 0;
};
