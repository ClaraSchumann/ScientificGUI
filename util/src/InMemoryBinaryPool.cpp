#include <filesystem>

#include <boost/interprocess/shared_memory_object.hpp>

#include "InMemoryBinaryPool.h"

using namespace boost::interprocess;

using namespace std::filesystem;

InMemoryBinaryPool::InMemoryBinaryPool(){};

InMemoryBinaryPool::~InMemoryBinaryPool(){};

int InMemoryBinaryPool::create_InMemoryBinary(const std::string &id,
                                               const std::string &output_path) {
  InMemoryBinary *p_obj = m_file_list.get_raw();
  int index = m_file_list.get_index(p_obj);

  // Determine whether to write out and check the validity.
  bool to_write_out = !output_path.empty();
  if (to_write_out) {
    try {
      path check(output_path);
      path dir_check = check.parent_path();
      if (!is_directory(dir_check)) {
        create_directory(dir_check);
      }
    } catch (...) {
      throw std::runtime_error("Invalid path to write out.\n");
    };
  }

  // Create shared_memory_object
  if (to_write_out) {
    char(*output_path_temp)[output_path_length] = m_output_path_list.get();
    if (!output_path_temp) {
      throw std::runtime_error("Output path list is fully allocated.\n");
    }
    std::strncpy(*output_path_temp, output_path.c_str(), output_path_length);
    new (p_obj) InMemoryBinary(index, id,
                               m_output_path_list.get_index(output_path_temp));
  } else {
    new (p_obj) InMemoryBinary(index, id);
  }

  return index;
};

InMemoryBinary& InMemoryBinaryPool::retrieve_InMemoryBinary(int index){
  return m_file_list[index];
};

void InMemoryBinaryPool::remove_InMemoryBinary(int index){
  m_output_path_list.release(m_file_list[index].m_output_path_entry_index);
  m_file_list.release(index);

  return;
};
