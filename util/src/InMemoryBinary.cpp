#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>

#include "InMemoryBinary.h"

#define DEBUG_INMEMORYBINARY
// #undef DEBUG_INMEMORYBINARY

using namespace boost::interprocess;

InMemoryBinary::InMemoryBinary(int file_entry_index, const std::string &id,
                               int output_path_entry_index)
    : m_output_path_entry_index(output_path_entry_index), m_total_size(0),
      m_file_entry_index(file_entry_index), m_used_size(0),
      m_readonly_accessor_count(0), m_being_written(false) {
  // After the destruction of object `shm`, the shared memory is still residing
  // in kernel.

#ifdef DEBUG_INMEMORYBINARY
  if (shared_memory_object::remove(id.c_str())) {
    std::cout << "Shared memory object [" << id
              << "] has not been properly release in last run.\n";
  }
#endif

  std::strncpy(m_id, id.c_str(), InMemoryBinary_id_length);
  shared_memory_object shm(create_only, id.c_str(), read_write);
};

InMemoryBinary::~InMemoryBinary() {
  std::cout << "Removing InMemoryBinary [" << m_id << "]\n";
  shared_memory_object::remove(m_id);
};

bool InMemoryBinary::allocated() { return m_total_size != 0; }

std::string InMemoryBinary::id() const { return std::string(m_id); }

size_t InMemoryBinary::total_size() const { return m_total_size; }

void InMemoryBinary::set_total_size(size_t total_size) {
  m_total_size = total_size;
}

size_t InMemoryBinary::used_size() const { return m_used_size; }

void InMemoryBinary::set_used_size(size_t used_size) {
  m_used_size = used_size;
}

bool InMemoryBinary::is_being_written() { return m_being_written; }

void InMemoryBinary::start_write() { m_being_written = true; }

void InMemoryBinary::stop_write() { m_being_written = false; }

void InMemoryBinary::start_read() { ++m_readonly_accessor_count; }

void InMemoryBinary::stop_read() { --m_readonly_accessor_count; }

bool InMemoryBinary::is_in_use() {
  return m_being_written || m_readonly_accessor_count;
}
