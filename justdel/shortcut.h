// Copyright 2017 jeonghun

#ifndef JUSTDEL_SHORTCUT_H_
#define JUSTDEL_SHORTCUT_H_

#include <windows.h>

class shortcut final {
 public:
  explicit shortcut(const wchar_t *path);
  ~shortcut() = default;

  bool create(const wchar_t *src_path, const wchar_t *icon_path, int32_t icon_idx) const;
  bool remove() const;
  bool chk_exist() const;

 private:
  wchar_t path_[MAX_PATH] = { 0 };
};

#endif  // JUSTDEL_SHORTCUT_H_
