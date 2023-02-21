#pragma once

#include <optional>
#include <stddef.h>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

class InMemoryBinary;

class InMemoryBinaryAccessor {
public:
  InMemoryBinaryAccessor(InMemoryBinary &file, bool writable);
  ~InMemoryBinaryAccessor();

  // Resize (possibly enlarge) the size of underlying shared memory in
  // kernel to accommodate at least 'size' bytes.
  void fallocate(size_t size);

  // Reassign the valid length of binary file residing in the underlying shared
  // memory object
  void ftruncate(size_t size);

  void fread(void *des, size_t size, size_t nmemb);
  void fwrite(void *des, size_t size, size_t nmemb);
  size_t ftell();
  void fseek(size_t pos);

protected:
  boost::interprocess::mode_t open_mode();

private:
  InMemoryBinary &m_file;
  bool m_writable;

  size_t m_pos; // Next position to read or write.

  boost::interprocess::shared_memory_object m_shm;
  std::optional<boost::interprocess::mapped_region> m_mr;
  char *m_data;
};
