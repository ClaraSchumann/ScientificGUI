#pragma once

#include <stddef.h>

#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

// To share between processes, we cannot rely on any raw pointers.
// We thus introduce an array that can automatically reuse the pre-allocated
// on-stack space.
template <typename T, size_t N> class SmartArray {
public:
  SmartArray() : m_tracker_begin(0), m_tracker_end(N - 1), m_remain(N) {
    for (int i = 0; i < N - 1; ++i) {
      m_tracker[i] = i + 1;
      m_in_use[i] = false;
    }

    m_tracker[N - 1] = -1;
    m_in_use[N - 1] = false;
  };

  ~SmartArray() {
    for (int i = 0; i < N; ++i) {
      if (m_in_use[i]) {
        destruct_object(reinterpret_cast<T *>(m_data + i * sizeof(T)));
      }
    };

    return;
  };

  // Return an memory arena without constructing the object.
  T *get_raw() {
    // TODO: decide whether to throw an exception.
    if (m_tracker_begin == -1) {
      return nullptr;
    }

    char *t = m_data + m_tracker_begin * sizeof(T);
    m_in_use[m_tracker_begin] = true;
    m_tracker_begin = m_tracker[m_tracker_begin];

    --m_remain;

    return reinterpret_cast<T *>(t);
  };

  template <typename... Args> T *get(Args &&...args) {
    char *t = reinterpret_cast<char *>(get_raw());

    T *ret = nullptr;
    if constexpr (sizeof...(Args)) {
      ret = reinterpret_cast<T *>(new (t) T(std::forward<Args>(args)...));
    } else {
      ret = reinterpret_cast<T *>(new (t) T);
    }

    return ret;
  };

  int get_index(T *p_obj) {
    return (reinterpret_cast<char *>(p_obj) - m_data) / sizeof(T);
  }

  void release_update_tracker(int index) {
    int m_tracked_end_t = m_tracker_end;
    m_tracker[index] = -1;
    m_tracker_end = index;
    m_in_use[index] = false;

    if (m_tracker_begin == -1) {
      m_tracker_begin = index;
    } else {
      m_tracker[m_tracked_end_t] = index;
    }

    ++m_remain;

    return;
  }

  void destruct_object(T *p_obj) {
    if constexpr (std::is_class_v<T>) {
      // void* temp = p_obj;
      // temp.~T();
      (*p_obj).~T();
    } else {
      std::memset(p_obj, 0, sizeof(T));
    }

    return;
  };

  void release(T *p_obj) {
    int index = (reinterpret_cast<char *>(p_obj) - m_data) / sizeof(T);

    destruct_object(p_obj);
    release_update_tracker(index);
  };

  void release(int index) {
    T *p_obj = reinterpret_cast<T *>(m_data + sizeof(T) * index);

    destruct_object(p_obj);
    release_update_tracker(index);
  }

  std::vector<int> get_blocks_in_use_array() {
    std::vector<int> in_use_array;
    for (int i = 0; i < N; ++i) {
      if (m_in_use[i]) {
        in_use_array.push_back(i);
      }
    };

    return in_use_array;
  };

  std::vector<int> get_chained_free_blocks_array() {
    std::vector<int> free_blocks;
    int temp = m_tracker_begin, count = 0;
    while (temp != -1) {
      free_blocks.push_back(temp);
      temp = m_tracker[temp];
      ++count;
    }

    return free_blocks;
  };

  T &operator[](int index) {
    T *p_obj = reinterpret_cast<T *>(m_data + sizeof(T) * index);
    return *p_obj;
  }

private:
  char m_data[N * sizeof(T)];

  int m_tracker[N]; // track all the unused position.
  int m_tracker_begin, m_tracker_end;
  int m_remain;

  bool m_in_use[N];
};
