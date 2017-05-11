// Copyright 2016 jeonghun

#ifndef JUSTDEL_EXTPATH_H_
#define JUSTDEL_EXTPATH_H_

#include <climits>
#include <cstdint>

class extpath final {
 public:
  explicit extpath(const wchar_t *path);
  ~extpath() = default;

  inline operator const wchar_t*() const { return path_buf_; }
  inline const wchar_t* get_normal_path() const { return path_buf_ + PATH_PREPEND_LEN; }
  bool extend(const wchar_t *path);
  bool reduce();

 private:
  constexpr static const wchar_t PATH_PREPEND[] = L"\\\\?\\";
  enum { PATH_PREPEND_LEN = _countof(PATH_PREPEND) - 1 };

  wchar_t path_buf_[SHRT_MAX] = { 0 };
  uint32_t length_ = 0;
};

#endif  // JUSTDEL_EXTPATH_H_
