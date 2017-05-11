// Copyright 2016 jeonghun

#include "extpath.h"
#include <windows.h>

extpath::extpath(const wchar_t *path) {
  if (path != nullptr) {
    if (wcsncmp(path, PATH_PREPEND, PATH_PREPEND_LEN) == 0) {
      wcscpy_s(path_buf_, path);
    } else {
      wcscpy_s(path_buf_, PATH_PREPEND);
      wcscpy_s(path_buf_ + PATH_PREPEND_LEN,
        _countof(path_buf_) - PATH_PREPEND_LEN, path);
    }
    length_ = wcslen(path_buf_);
  }
}

bool extpath::extend(const wchar_t *path) {
  bool add = false;

  if (path != nullptr && length_ > 0) {
    const size_t extend_len = wcslen(path);
    if (extend_len > 0 && length_ + extend_len < _countof(path_buf_)) {
      if (path_buf_[length_ - 1] != L'\\') {
        path_buf_[length_++] = L'\\';
      }
      wcscpy_s(path_buf_ + length_, _countof(path_buf_) - length_, path);
      length_ += extend_len;
      add = true;
    }
  }

  return add;
}

bool extpath::reduce() {
  wchar_t *delim_ptr = path_buf_ + length_ - 1;
  while (delim_ptr > path_buf_ && *(--delim_ptr) != L'\\') {}
  *delim_ptr = L'\0';
  uint32_t prev_length = length_;
  length_ = delim_ptr - path_buf_;

  return prev_length > length_;
}
