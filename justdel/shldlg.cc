// Copyright 2016 jeonghun

#include "shldlg.h"
#include <atlcomcli.h>
#include <shlobj.h>

bool shldlg::show() {
  return thread_.create() && ready_.wait();
}

bool shldlg::close() {
  return thread_.chk_running() && thread_.post_message(REQ_CLOSE, 0, 0) &&
    ready_.wait() && thread_.close();
}

bool shldlg::set_location(const wchar_t *path) {
  return path != nullptr &&
    thread_.post_message(REQ_SET_LOCATION, reinterpret_cast<WPARAM>(path), 0) &&
    ready_.wait();
}

DWORD shldlg::thread_proc(LPVOID arg_ptr) {
  HRESULT hr = CoInitializeEx(NULL,
    COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  if (SUCCEEDED(hr)) {
    CComPtr<IOperationsProgressDialog> dlg_ptr;
    hr = dlg_ptr.CoCreateInstance(CLSID_ProgressDialog);
    if (SUCCEEDED(hr) && dlg_ptr != nullptr) {
      hr = dlg_ptr->StartProgressDialog(NULL, OPPROGDLG_DEFAULT);
      if (SUCCEEDED(hr) &&
          PostThreadMessage(GetCurrentThreadId(), REQ_INIT, 0, 0) != FALSE) {
        const UINT_PTR timer_id = SetTimer(NULL, 0, 250, nullptr);
        MSG msg = { 0 };
        while (GetMessage(&msg, NULL, 0, 0)) {
          if (msg.hwnd == NULL) {
            switch (msg.message) {
            case REQ_INIT:
              dlg_ptr->SetOperation(SPACTION_DELETING);
              dlg_ptr->SetMode(PDM_INDETERMINATE);
              ready_.set();
              break;

            case REQ_CLOSE:
              KillTimer(NULL, timer_id);
              dlg_ptr->StopProgressDialog();
              PostQuitMessage(0);
              break;

            case REQ_SET_LOCATION:
              {
                CComPtr<IShellItem> shlitm_ptr;
                SHCreateItemFromParsingName(
                  reinterpret_cast<const wchar_t*>(msg.wParam), nullptr,
                  IID_IShellItem, reinterpret_cast<void**>(&shlitm_ptr));
                if (shlitm_ptr != nullptr) {
                  dlg_ptr->UpdateLocations(shlitm_ptr, nullptr, nullptr);
                }
                ready_.set();
              }
              break;

            case WM_TIMER:
              if (timer_id == msg.wParam) {
                PDOPSTATUS status;
                if (SUCCEEDED(dlg_ptr->GetOperationStatus(&status))) {
                  user_cancel_ = status == PDOPS_CANCELLED;
                }
              }
              break;
            }
          }
        }
      }
    }
  }

  CoUninitialize();
  ready_.set();

  return SUCCEEDED(hr) ? 0 : -1;
}
