// Copyright 2016 jeonghun

#ifndef JUSTDEL_EVENT_H_
#define JUSTDEL_EVENT_H_

#include <windows.h>

class event final {
 public:
  event() : hevent_(CreateEvent(nullptr, FALSE, FALSE, nullptr)) {}
  ~event() { if (hevent_ != NULL) CloseHandle(hevent_); }

  inline bool set() { return hevent_ != NULL ?
    SetEvent(hevent_) != FALSE : false; }
  inline bool wait() { return hevent_ != NULL ?
    WaitForSingleObject(hevent_, INFINITE) == WAIT_OBJECT_0 : false; }

 private:
  HANDLE hevent_ = NULL;
};

#endif  // JUSTDEL_EVENT_H_
