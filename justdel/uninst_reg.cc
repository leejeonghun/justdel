// Copyright 2017 jeonghun

#include "uninst_reg.h"

uninst_reg::uninst_reg(const wchar_t *key_name) : regkey_{ 0 } {
  if (key_name != nullptr) {
    wcscpy_s(regkey_,
      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
    wcscat_s(regkey_, key_name);
  }
}

bool uninst_reg::create(const wchar_t *disp_name, const wchar_t *exe_path,
    const wchar_t *version, uint32_t size) const {
  bool create_reg = false;

  if (disp_name != nullptr && exe_path != nullptr && version != nullptr) {
    HKEY hkey = NULL;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, regkey_, 0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hkey,
        NULL) == ERROR_SUCCESS && hkey != NULL) {
      create_reg = set_regvalue(hkey, L"DisplayName", disp_name) &&
        set_regvalue(hkey, L"UninstallString", exe_path) &&
        set_regvalue(hkey, L"DisplayVersion", version) &&
        set_regvalue(hkey, L"EstimatedSize", size);
      RegCloseKey(hkey);
    }
  }

  return create_reg;
}

bool uninst_reg::remove() const {
  bool remove_reg = false;

  HKEY hkey = NULL;
  if (RegOpenKeyEx(HKEY_CURRENT_USER, regkey_, 0,
      KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS && hkey != NULL) {
    remove_reg = RegDeleteTree(hkey, nullptr) == ERROR_SUCCESS;
    RegCloseKey(hkey);
  }

  return remove_reg;
}

bool uninst_reg::chk_exist() const {
  bool exist_reg = false;

  HKEY hkey = NULL;
  if (RegOpenKeyEx(HKEY_CURRENT_USER, regkey_, 0,
      KEY_READ, &hkey) == ERROR_SUCCESS && hkey != NULL) {
    RegCloseKey(hkey);
    exist_reg = true;
  }

  return exist_reg;
}

bool uninst_reg::compare_bin_path(const wchar_t *bin_path) const {
  bool same_path = false;

  if (bin_path != nullptr) {
    HKEY hkey = NULL;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, regkey_, 0, KEY_READ,
      &hkey) == ERROR_SUCCESS && hkey != NULL) {
      wchar_t uninst_str[MAX_PATH] = { 0 };
      DWORD value_type = 0;
      DWORD value_size = sizeof(uninst_str);
      if (RegQueryValueEx(hkey, L"UninstallString", 0, &value_type,
          reinterpret_cast<BYTE*>(uninst_str), &value_size) == ERROR_SUCCESS) {
        same_path = _wcsicmp(uninst_str, bin_path) == 0;
      }
      RegCloseKey(hkey);
    }
  }

  return same_path;
}

bool uninst_reg::set_regvalue(HKEY hkey,
    const wchar_t *name, const wchar_t *value) const {
  return RegSetValueEx(hkey, name, 0, REG_SZ,
    reinterpret_cast<const BYTE*>(value),
    (wcslen(value) + 1) * sizeof(wchar_t)) == ERROR_SUCCESS;
}

bool uninst_reg::set_regvalue(HKEY hkey,
    const wchar_t *name, uint32_t value) const {
  return RegSetValueEx(hkey, name, 0, REG_DWORD,
    reinterpret_cast<const BYTE*>(&value),
    sizeof(value)) == ERROR_SUCCESS;
}
