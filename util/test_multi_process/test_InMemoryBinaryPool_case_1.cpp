#include "InMemoryBinaryPool.h"
#include "InterprocessSingleton.hpp"

#include <boost/interprocess/shared_memory_object.hpp>

int main(int argc, char** argv){
  auto master = [](){
      boost::interprocess::shared_memory_object::remove("test_pool");
      auto& pool_instance = InterprocessSingleton<InMemoryBinaryPool>::instance(
            InterprocessAccessRole::Owner, "test_pool");
      int temp_file_index = pool_instance.create_InMemoryBinary("test_file", "/home/dog/Documents/GAMMA_TMP_TEST");

    };

  if(argc == 1){}
  else if(argc == 2){

  }

  return 0;
}
