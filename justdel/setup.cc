// Copyright 2017 jeonghun

#include "setup.h"
#include "uninst_reg.h"
#include "shortcut.h"

#define APP_NAME L"JustDelete"

#pragma comment(lib, "version")

setup::setup() {
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

  wchar_t curr_bin_path[MAX_PATH] = { 0 };
  GetModuleFileName(NULL, curr_bin_path, _countof(curr_bin_path));
  const wchar_t *bin_name = PathFindFileName(curr_bin_path);

  wchar_t appdata_path[MAX_PATH] = { 0 };
  SHGetSpecialFolderPath(NULL, appdata_path, CSIDL_APPDATA, false);
  PathCombine(lnk_path_, appdata_path, L"Microsoft\\Windows\\SendTo");
  PathCombine(lnk_path_, lnk_path_, APP_NAME ".lnk");
  PathCombine(dst_path_, appdata_path, APP_NAME);
  PathCombine(bin_path_, dst_path_, bin_name);
}

setup::~setup() {
  CoUninitialize();
}

bool setup::run() const {
  bool success = false;

  const wchar_t *result_msg = nullptr;
  uninst_reg reg(UNINST_REG_KEY);
  shortcut send_to_lnk(lnk_path_);

  wchar_t exe_path[MAX_PATH] = { 0 };
  GetModuleFileName(NULL, exe_path, _countof(exe_path));

  if (reg.chk_exist() && reg.compare_bin_path(exe_path)) {
    if (IsUserAnAdmin() == FALSE && confirm_uninstall()) {
      success = ShellExecute(GetForegroundWindow(), L"runas", bin_path_,
        nullptr, nullptr, SW_SHOWNORMAL) != FALSE;
    } else {
      success = IsUserAnAdmin() != FALSE &&
        (send_to_lnk.chk_exist() ? send_to_lnk.remove() : true) &&
        remove_app() && reg.remove();
      result_msg = success ? L"Uninstall Complete." : L"Uninstall Failed.";
    }
  } else {
    SHSTOCKICONINFO sii = { sizeof(sii), 0 };
    if (SUCCEEDED(SHGetStockIconInfo(SIID_RECYCLER, SHGSI_ICONLOCATION, &sii))) {
      success = install_app() &&
        (send_to_lnk.chk_exist() ? true : send_to_lnk.create(bin_path_, sii.szPath, sii.iIcon)) &&
        reg.create(APP_NAME, bin_path_, get_bin_version().c_str(), get_bin_size_in_kb());
      result_msg = success ? L"Install Complete." : L"Install Failed.";
    }
  }

  if (result_msg != nullptr) {
    const wchar_t *mb_title = L"Success";
    DWORD mb_type = MB_OK;
    if (success == false) {
      mb_title = L"Error";
      mb_type |= MB_ICONERROR;
    }
    MessageBox(GetForegroundWindow(), result_msg, mb_title, mb_type);
  }

  return success;
}

bool setup::install_app() const {
  wchar_t src_path[MAX_PATH] = { 0 };
  GetModuleFileName(NULL, src_path, _countof(src_path));

  return (CreateDirectory(dst_path_, NULL) != FALSE ||
      GetLastError() == ERROR_ALREADY_EXISTS) &&
    CopyFile(src_path, bin_path_, FALSE) != FALSE;
}

std::wstring setup::get_bin_version() const {
  enum { MAX_VERSION_LENGTH = 5 + 1 * 4 };  // USHRT_MAX_LEN(5) + DOT(1) * 4

  wchar_t ver_string[MAX_VERSION_LENGTH] = { 0 };

  HMODULE module_handle = GetModuleHandle(nullptr);
  HRSRC res_handle = FindResource(module_handle, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
  if (res_handle != NULL) {
    HGLOBAL global_handle = LoadResource(module_handle, res_handle);
    uint32_t rsc_size = SizeofResource(module_handle, res_handle);
    const uint8_t *rsc_ptr = reinterpret_cast<uint8_t*>(LockResource(global_handle));

    if (rsc_size > 0 && rsc_ptr != nullptr) {
      VS_FIXEDFILEINFO *info_ptr = nullptr;
      uint32_t info_len = 0;
      if (VerQueryValue(rsc_ptr, L"\\", reinterpret_cast<void**>(&info_ptr), &info_len) != FALSE) {
        const uint32_t hi_ver = info_ptr->dwFileVersionMS;
        const uint32_t lo_ver = info_ptr->dwFileVersionLS;
        swprintf_s(ver_string, L"%u.%u.%u.%u",
          HIWORD(hi_ver), LOWORD(hi_ver), HIWORD(lo_ver), LOWORD(lo_ver));
      }
    }
  }

  return ver_string;
}

uint32_t setup::get_bin_size_in_kb() const {
  enum { KILO_UNIT = 1024 };

  uint32_t bin_size = 0;

  wchar_t src_path[MAX_PATH] = { 0 };
  GetModuleFileName(NULL, src_path, _countof(src_path));

  WIN32_FILE_ATTRIBUTE_DATA fad = { 0 };
  if (GetFileAttributesEx(src_path, GetFileExInfoStandard, &fad) != FALSE) {
    bin_size = fad.nFileSizeLow / KILO_UNIT;
  }

  return bin_size;
}

bool setup::confirm_uninstall() const {
  bool confirm = false;

  HHOOK hook_handle = SetWindowsHookEx(WH_CBT,
    [](int code, WPARAM wparam, LPARAM lparam)->LRESULT {
      static bool set_shield = false;
      if (set_shield == false && code == HCBT_ACTIVATE) {
        Button_SetElevationRequiredState(
          GetDlgItem(reinterpret_cast<HWND>(wparam), IDYES), true);
        set_shield = true;
      }
      return CallNextHookEx(NULL, code, wparam, lparam);
    }, NULL, GetCurrentThreadId());

  confirm = MessageBox(GetForegroundWindow(),
    L"Are you sure you want to uninstall " APP_NAME,
    L"Confirm", MB_YESNO | MB_ICONWARNING) == IDYES;

  UnhookWindowsHookEx(hook_handle);

  return confirm;
}

bool setup::remove_app() const {
  bool remove = false;

  wchar_t rename[MAX_PATH] = { 0 };
  if (rename_for_delete(rename)) {
    remove =
      MoveFileEx(rename, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT) != FALSE &&
      MoveFileEx(dst_path_, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT) != FALSE;
  }

  return remove;
}

template <typename T>
bool setup::rename_for_delete(T& rename) const {
  wcscpy_s(rename, bin_path_);

  wchar_t index[9] = { 0 };
  for (int32_t i = 0; i < MAXINT32; i++) {
    _itow_s(i, index, 16);
    PathRemoveExtension(rename);
    PathAddExtension(rename, index);
    if (PathFileExists(rename) == FALSE) {
      break;
    }
  }

  SHFILEOPSTRUCT file_op = { NULL, FO_RENAME, bin_path_, rename,
    FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT, false, nullptr, nullptr };
  return SHFileOperation(&file_op) == 0;
}
