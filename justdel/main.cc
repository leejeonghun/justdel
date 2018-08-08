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
  if (cmdline != nullptr && cmdline[0] != L'\0') {
    int argc = 0;
    wchar_t **argv = CommandLineToArgvW(cmdline, &argc);
    if (argv != nullptr) {
      size_t total_deleted = 0;
      size_t total_failed = 0;

      shldlg dialog;
      if (dialog.show()) {
        for (int i = 0; i < argc; i++) {
          PathUnquoteSpaces(argv[i]);
          if (PathFileExists(argv[i]) != FALSE && dialog.set_location(argv[i])) {
            file_enumerator files_in_dir(argv[i]);
            file_deleter deleter;
            file_enumerator::info_t info;
            while (dialog.chk_canceled() == false && files_in_dir.next(&info)) {
              deleter.erase(info);
            }
            total_deleted += deleter.get_num_of_deleted();
            total_failed += deleter.get_num_of_failed();
          }
        }
        dialog.close();
        if (total_failed > 0) {
          MessageBox(NULL, format_string(L"Suceeded : %d\r\nFailed : %d",
            total_deleted, total_failed).c_str(),
            L"Result", MB_OK | MB_ICONWARNING);
        }
      }
      LocalFree(argv);
    }
  } else {
    setup install;
    install.run();
  }

  return 0;
}
