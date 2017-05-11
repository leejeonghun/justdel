// Copyright 2016 jeonghun

#ifndef JUSTDEL_SHLDLG_H_
#define JUSTDEL_SHLDLG_H_

#include <windows.h>
#include "w32thread.h"
#include "event.h"

class shldlg final {
 public:
  shldlg() : thread_(this, &shldlg::thread_proc) {}
  ~shldlg() = default;

  bool show();
  bool close();
  bool set_location(const wchar_t *path);
  inline bool chk_canceled() const { return user_cancel_; }

 private:
  enum {
    REQ_INIT = 0,
    REQ_CLOSE,
    REQ_SET_LOCATION,
  };

  DWORD thread_proc(LPVOID arg_ptr);

  event ready_;
  w32thread<shldlg> thread_;
  volatile bool user_cancel_ = false;
};

#endif  // JUSTDEL_SHLDLG_H_
