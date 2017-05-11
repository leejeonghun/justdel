// Copyright 2017 jeonghun

#include "shortcut.h"
#include <shlwapi.h>
#include <atlcomcli.h>

#pragma comment(lib, "shlwapi")

shortcut::shortcut(const wchar_t *path) {
  if (path != nullptr) {
    wcscpy_s(path_, path);
  }
}

bool shortcut::create(const wchar_t *src_path, const wchar_t *icon_path, int32_t icon_idx) const {
  bool create_shortcut = false;

  if (PathFileExists(src_path) && PathIsRelative(src_path) == FALSE &&
      PathFileExists(icon_path) && PathIsRelative(icon_path) == FALSE) {
    CComPtr<IShellLink> shell_ptr;
    if (SUCCEEDED(shell_ptr.CoCreateInstance(CLSID_ShellLink))) {
      wchar_t work_path[MAX_PATH] = { 0 };
      wcscpy_s(work_path, src_path);
      PathRemoveFileSpec(work_path);

      if (SUCCEEDED(shell_ptr->SetPath(src_path)) &&
          SUCCEEDED(shell_ptr->SetIconLocation(icon_path, icon_idx)) &&
          SUCCEEDED(shell_ptr->SetWorkingDirectory(work_path))) {
        CComQIPtr<IPersistFile> persist_file_ptr(shell_ptr);
        if (persist_file_ptr != nullptr) {
          create_shortcut =
            SUCCEEDED(persist_file_ptr->Save(path_, true));
        }
      }
    }
  }

  return create_shortcut;
}

bool shortcut::remove() const {
  return DeleteFile(path_) != FALSE;
}

bool shortcut::chk_exist() const {
  return PathFileExists(path_) != FALSE;
}
