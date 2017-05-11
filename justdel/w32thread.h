// Copyright 2016 jeonghun

#ifndef JUSTDEL_W32THREAD_H_
#define JUSTDEL_W32THREAD_H_

#include <windows.h>

template <typename T>
class w32thread {
 public:
  typedef w32thread<T> self_t;
  typedef DWORD(T::*thread_proc_t)(LPVOID param_ptr);

  w32thread(T *obj_ptr, thread_proc_t proc_ptr);
  virtual ~w32thread();

  bool create(void *arg_ptr = nullptr);
  bool close(bool wait = true);
  bool chk_running() const { return run_; }
  bool post_message(UINT msg, WPARAM wparam, LPARAM lparam);

 private:
  T *obj_ptr_;
  thread_proc_t proc_ptr_;
  HANDLE handle_ = NULL;
  DWORD id_ = 0;
  volatile bool run_ = false;
  void *arg_ptr_ = nullptr;
};

#include "w32thread-inl.h"

#endif  // JUSTDEL_W32THREAD_H_
