// Copyright 2017 jeonghun

#include <windows.h>
#include <shlwapi.h>
#include "shldlg.h"
#include "setup.h"
#include "file_enumerator.h"
#include "file_deleter.h"
#include "format_string.h"

#pragma comment(lib, "shlwapi")

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR cmdline, int) {
  if (PathFileExists(cmdline) != FALSE) {
    shldlg dialog;
    if (dialog.show() && dialog.set_location(cmdline)) {
      file_enumerator files_in_dir(cmdline);
      file_deleter deleter;
      file_enumerator::info_t info;
      while (dialog.chk_canceled() == false && files_in_dir.next(&info)) {
        deleter.erase(info);
      }
      dialog.close();
      if (deleter.get_num_of_failed() > 0) {
        MessageBox(NULL, format_string(L"Suceeded : %d\r\nFailed : %d",
            deleter.get_num_of_deleted(), deleter.get_num_of_failed()).c_str(),
          L"Result", MB_OK | MB_ICONWARNING);
      }
    }
  } else {
    setup install;
    install.run();
  }

  return 0;
}
