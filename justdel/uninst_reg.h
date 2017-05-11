// Copyright 2017 jeonghun

#ifndef JUSTDEL_UNINST_REG_H_
#define JUSTDEL_UNINST_REG_H_

#include <windows.h>

class uninst_reg final {
 public:
  explicit uninst_reg(const wchar_t *key_name);
  ~uninst_reg() = default;

  bool create(const wchar_t *disp_name, const wchar_t *exe_path,
    const wchar_t *version, uint32_t size) const;
  bool remove() const;
  bool chk_exist() const;
  bool compare_bin_path(const wchar_t *bin_path) const;

 private:
  // https://msdn.microsoft.com/ko-kr/library/windows/desktop/ms724872(v=vs.85).aspx
  enum { REGKEY_MAX_LENGTH = 255 + 1 };

  inline bool set_regvalue(HKEY hkey,
    const wchar_t *name, const wchar_t *value) const;
  inline bool set_regvalue(HKEY hkey,
    const wchar_t *name, uint32_t value) const;

  wchar_t regkey_[REGKEY_MAX_LENGTH];
};

#endif  // JUSTDEL_UNINST_REG_H_
