#pragma once

#include <atomic>
#include <string>
#include <stddef.h>

static constexpr size_t InMemoryBinary_id_length = 128;

class InMemoryBinaryPool;

// Instances of this class will be shared among processes.
// No direct or indirect pointers, no reference, no virtual table.
class InMemoryBinary {
  friend InMemoryBinaryPool;
public:
  InMemoryBinary(int file_entry_index, const std::string& id, int output_path_entry_index = -1);
  ~InMemoryBinary();

  bool allocated();

  std::string id() const;

  size_t total_size() const;
  void set_total_size(size_t total_size);

  size_t used_size() const;
  void set_used_size(size_t used_size);

  bool is_being_written();
  void start_write();
  void stop_write();

  void start_read();
  void stop_read();

  bool is_in_use();

private:
  char m_id[InMemoryBinary_id_length];

  size_t m_total_size;
  size_t m_used_size;

  int m_output_path_entry_index;

  // Index of *this in the InMemoryBinaryPool
  int m_file_entry_index;

  std::atomic<int> m_readonly_accessor_count;
  std::atomic<bool> m_being_written;
};
