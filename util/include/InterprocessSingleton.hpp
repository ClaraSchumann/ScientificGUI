#pragma once

#include <exception>
#include <utility>
#include <optional>
#include <iostream>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "InterprocessAccessRole.h"

// Instance of type T should contain NO POINTERS, NO VIRTUAL TABLES AND NO REFERENCES.
template <typename T> class InterprocessSingleton {
protected:
  template <typename ...Args>
  InterprocessSingleton(InterprocessAccessRole role, const std::string &id, Args&& ... args)
      : m_role(role), m_id(id) {
    if(m_role == InterprocessAccessRole::Owner){
        size_t page_size = boost::interprocess::mapped_region::get_page_size();
        size_t length = ((sizeof(T) - 1) / page_size + 1) * page_size;
        m_shm = boost::interprocess::shared_memory_object(
              boost::interprocess::create_only,
              id.c_str(),
              boost::interprocess::read_write);
        m_shm.truncate(length);
        m_mr = boost::interprocess::mapped_region(m_shm, boost::interprocess::read_write);
        p_obj = (T*) m_mr->get_address();
      }
    else if(m_role == InterprocessAccessRole::Guest) {
        m_shm = boost::interprocess::shared_memory_object(
              boost::interprocess::open_only,
              id.c_str(),
              boost::interprocess::read_write);
      }

    m_mr = boost::interprocess::mapped_region(m_shm, boost::interprocess::read_write);
    p_obj = (T*) m_mr->get_address();

    if(m_role == InterprocessAccessRole::Owner){
        if constexpr(sizeof...(args)){
          new (p_obj) T(std::forward<Args>(args)...);
        }
        else{
          new (p_obj) T();
        }
      };
  };

public:
  ~InterprocessSingleton(){
    if(m_role == InterprocessAccessRole::Owner){
        if constexpr(std::is_class_v<T>) {
          p_obj->~T();
          m_mr.reset();
          bool success = boost::interprocess::shared_memory_object::remove(m_id.c_str());
          std::cout << "Remove Interprocess Object [" << m_id << "] : " << success << std::endl;
        }
      };

    m_mr.reset();
  };

  template <typename ...Args>
  static T& instance(InterprocessAccessRole role, const std::string &id, Args&& ... args){
    if constexpr(sizeof...(Args)) {
      static InterprocessSingleton<T> m_instance(role, id, std::forward<Args>(args)...);
      return *(m_instance.p_obj);
    }
    else{
      static InterprocessSingleton<T> m_instance(role, id);
      return *(m_instance.p_obj);
    }
  };

  T* operator->() const{
    return p_obj;
  }

private:
  // Branch the behaviour of Destructor.
  InterprocessAccessRole m_role;

  // This class is not meant to be interprocess itself,
  // So it is SAFE to use std::string.
  std::string m_id;

  // The doc claims that `mapped_region` can be valid after the destruction
  // of its corresponding `shared_memory_object`. But we won't take this risk.
  boost::interprocess::shared_memory_object m_shm;
  // To destruct mapped_region manually.
  std::optional<boost::interprocess::mapped_region> m_mr;
  T *p_obj;
};
