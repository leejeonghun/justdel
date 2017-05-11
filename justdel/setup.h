// Copyright 2017 jeonghun

#ifndef JUSTDEL_SETUP_H_
#define JUSTDEL_SETUP_H_

#include <windows.h>
#include <cstdint>
#include <string>

class setup final {
 public:
  setup();
  ~setup();

  bool run() const;

 private:
  constexpr static const wchar_t UNINST_REG_KEY[] = L"{EC47ECDB-B232-4B27-AD9C-44B964340D74}";
  
  bool install_app() const;
  std::wstring get_bin_version() const;
  uint32_t get_bin_size_in_kb() const;
  bool confirm_uninstall() const;
  bool remove_app() const;
  template <typename T> bool rename_for_delete(T& rename) const;

  wchar_t dst_path_[MAX_PATH] = { 0 };
  wchar_t bin_path_[MAX_PATH] = { 0 };
  wchar_t lnk_path_[MAX_PATH] = { 0 };
};

#endif  // JUSTDEL_SETUP_H_
