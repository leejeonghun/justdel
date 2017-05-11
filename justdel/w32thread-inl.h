// Copyright 2016 jeonghun

#ifndef JUSTDEL_W32THREAD_INL_H_
#define JUSTDEL_W32THREAD_INL_H_

#include "w32thread.h"
#include <cassert>

template <typename T>
w32thread<T>::w32thread(T *obj_ptr, thread_proc_t proc_ptr)
    : obj_ptr_(obj_ptr)
    , proc_ptr_(proc_ptr) {
  assert(obj_ptr != nullptr);
  assert(proc_ptr != nullptr);
}

template <typename T>
w32thread<T>::~w32thread() {
  close(false);
}

template <typename T>
bool w32thread<T>::create(void *arg_ptr) {
  bool succeeded = false;

  if (handle_ == NULL) {
    arg_ptr_ = arg_ptr;
    handle_ = CreateThread(nullptr, 0, [](void *arg_ptr)->DWORD {
      DWORD retcode = -1;
      self_t* this_ptr = reinterpret_cast<self_t*>(arg_ptr);
      if (this_ptr != nullptr) {
        this_ptr->run_ = true;
        retcode =
          ((this_ptr->obj_ptr_)->*(this_ptr->proc_ptr_))(this_ptr->arg_ptr_);
        this_ptr->run_ = false;  // TODO(jeonghun): after deleting an object, a crash will occur.
      }
      return retcode;
    }, this, 0, &id_);
    succeeded = handle_ != NULL;
  }

  return succeeded;
}

template <typename T>
bool w32thread<T>::close(bool wait) {
  if (handle_ != NULL) {
    if (wait) {
      WaitForSingleObject(handle_, INFINITE);
    }
    CloseHandle(handle_);
    handle_ = NULL;
  }

  return handle_ == NULL;
}

template<typename T>
bool w32thread<T>::post_message(UINT msg, WPARAM wparam, LPARAM lparam) {
  return id_ != 0 ?
    PostThreadMessage(id_, msg, wparam, lparam) != FALSE : false;
}

#endif  // JUSTDEL_W32THREAD_INL_H_
