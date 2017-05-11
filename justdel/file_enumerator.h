// Copyright 2016 jeonghun

#ifndef JUSTDEL_FILE_ENUMERATOR_H_
#define JUSTDEL_FILE_ENUMERATOR_H_

#include <windows.h>
#include <cstdint>
#include <list>
#include "extpath.h"

class file_enumerator {
 public:
  struct info_t {
    wchar_t path[SHRT_MAX];
    uint32_t attr;
  };

  explicit file_enumerator(const wchar_t *path);
  virtual ~file_enumerator() = default;

  bool next(info_t *fi_ptr);

 private:
  inline bool is_dir(const WIN32_FIND_DATA &wfd) const { return !!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY); }
  inline bool is_period_path(const WIN32_FIND_DATA &wfd) const;

  std::list<HANDLE> hfind_;
  std::list<DWORD> find_attr_;
  extpath path_;
};

#endif  // JUSTDEL_FILE_ENUMERATOR_H_
