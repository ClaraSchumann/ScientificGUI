#pragma once

#include <cstddef>

#include "SmartArray.hpp"
#include "InMemoryBinary.h"

static constexpr size_t output_path_length = 512;
static constexpr size_t output_file_max_num = 1024;
static constexpr size_t all_file_max_num = 4096;

// Entries in this object should be strictly mapped to shared_memory resources
// residing in kernel.
class InMemoryBinaryPool {
public:
  InMemoryBinaryPool();
  ~InMemoryBinaryPool();

  InMemoryBinaryPool(const InMemoryBinaryPool&) = delete;
  InMemoryBinaryPool& operator=(const InMemoryBinaryPool&) = delete;

  // Output path must be an absolute path;
  int create_InMemoryBinary(const std::string& id, const std::string& output_path = "");

  // Decouple the creation and retrieval of InMemoryBinary instance.
  InMemoryBinary& retrieve_InMemoryBinary(int index);
  void remove_InMemoryBinary(int index);

private:
  SmartArray<char[output_path_length], output_file_max_num> m_output_path_list;
  SmartArray<InMemoryBinary, all_file_max_num> m_file_list;
};
