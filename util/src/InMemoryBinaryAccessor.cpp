#include <iostream>

#include "InMemoryBinary.h"
#include "InMemoryBinaryAccessor.h"

using namespace boost::interprocess;

// There are two enums, `::mode_t` and `::boost::interprocess::mode_t`, SO BE
// VERY CAREFUL!
boost::interprocess::mode_t InMemoryBinaryAccessor::open_mode() {
  if (m_writable) {
    return boost::interprocess::read_write; // as distinguished from
                                            // `::read_write` from "sys/type.h"
  } else {
    return boost::interprocess::read_only;
  }
};

InMemoryBinaryAccessor::InMemoryBinaryAccessor(InMemoryBinary &file,
                                               bool writable)
    : m_file(file), m_writable(writable), m_pos(0),
      m_shm(open_only, m_file.id().c_str(), open_mode()),
      m_mr(mapped_region(m_shm, open_mode())),
      m_data(m_file.allocated() ? static_cast<char *>(m_mr->get_address())
                                : nullptr) {
  if (m_file.is_being_written()) {
    throw std::runtime_error("A writable accessor already exists.");
  }

  if (m_writable) {
    m_file.start_write();
  } else {
    m_file.start_write();
  }
};

InMemoryBinaryAccessor::~InMemoryBinaryAccessor() {
  if (m_writable) {
    m_file.stop_write();
  } else {
    m_file.stop_read();
  }
};

void InMemoryBinaryAccessor::fallocate(size_t size) {
  if (!m_writable) {
    throw std::runtime_error(
        "Cannot truncate underlying file through readonly accessor.\n");
  }

  size = ((size - 1) / mapped_region::get_page_size() + 1) *
         mapped_region::get_page_size();
  if (size == m_file.total_size()) {
    return;
  }

  size_t backup_length = m_file.used_size();
  if (m_file.allocated()) {
    if (m_file.used_size() > size) {
      m_file.set_used_size(size);
      backup_length = size;
      std::cout << "New shared_memory section cannot accmmodate "
                   "the pre-existing contents. Discard trailing parts.\n";
    };
    // TODO: print warning message using some logger.
    std::cout << "Is reallocating the underlying shared_memory in kernel, make "
                 "sure that this current accessor is unique system-wide.\n";
  }

  char *backup = nullptr;
  if (backup_length) {
    backup = new char[backup_length];
    std::memcpy(backup, m_data, backup_length);
  }

  m_shm.truncate(size);
  m_file.set_total_size(size);

  m_mr.reset();
  m_mr = mapped_region(m_shm, open_mode());
  m_data = static_cast<char *>(m_mr->get_address());

  if (backup) {
    std::memcpy(m_data, backup, backup_length);
    delete[] backup;
  }

  return;
}

void InMemoryBinaryAccessor::ftruncate(size_t size) {
  fallocate(size);
  m_file.set_used_size(size);

  return;
};

void InMemoryBinaryAccessor::fread(void *des, size_t size, size_t nmemb) {
  size_t new_pos = m_pos + size * nmemb;
  if (new_pos > m_file.used_size()) {
    // TODO: include more details in these error messages.
    throw std::runtime_error("End of file reached!");
  };

  std::memcpy(des, m_data + m_pos, size * nmemb);
  m_pos = new_pos;

  return;
};

void InMemoryBinaryAccessor::fwrite(void *src, size_t size, size_t nmemb) {
  size_t new_pos = m_pos + size * nmemb;
  if (new_pos > m_file.total_size()) {
    // TODO: include more details in these error messages.
    throw std::runtime_error("Exceed the boundary of buffer section!");
  };

  std::memcpy(m_data + m_pos, src, size * nmemb);
  m_pos = new_pos;
  if (m_pos > m_file.used_size()) {
    m_file.set_used_size(m_pos);
  }

  return;
};

size_t InMemoryBinaryAccessor::ftell() { return m_pos; }

void InMemoryBinaryAccessor::fseek(size_t pos) {
  if (pos > m_file.used_size()) {
    throw std::runtime_error("Target binary  is out-of-boundary.");
  }

  m_pos = pos;
};
