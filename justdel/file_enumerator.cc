// Copyright 2016 jeonghun

#include "file_enumerator.h"

file_enumerator::file_enumerator(const wchar_t *path) : path_(path) {
  const uint32_t attr = GetFileAttributes(path_);
  if (attr != INVALID_FILE_ATTRIBUTES) {
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
      hfind_.push_back(NULL);
    } else {
      WIN32_FIND_DATA wfd = { 0 };
      HANDLE hfile = FindFirstFile(path_, &wfd);
      if (hfile != INVALID_HANDLE_VALUE) {
        hfind_.push_back(hfile);
        find_attr_.push_back(wfd.dwFileAttributes);
      }
    }
  }
}

bool file_enumerator::next(info_t *fi_ptr) {
  bool find = false;

  if (hfind_.empty() == false && fi_ptr != nullptr) {
    WIN32_FIND_DATA wfd;
    if (hfind_.back() == NULL) {
      path_.extend(L"*");
      HANDLE hfile = FindFirstFile(path_, &wfd);
      if (hfile != INVALID_HANDLE_VALUE) {
        hfind_.back() = hfile;
        find_attr_.push_back(wfd.dwFileAttributes);
        find = true;
      }
      path_.reduce();
    } else {
      find = FindNextFile(hfind_.back(), &wfd) != FALSE;
    }

    if (find) {
      if (is_dir(wfd)) {
        if (is_period_path(wfd) == false) {
          hfind_.push_back(NULL);
          path_.extend(wfd.cFileName);
        }
        find = next(fi_ptr);
      } else {
        path_.extend(wfd.cFileName);
        wcscpy_s(fi_ptr->path, path_);
        fi_ptr->attr = wfd.dwFileAttributes;
        path_.reduce();
      }
    } else if (hfind_.back() != NULL) {
      wcscpy_s(fi_ptr->path, path_);
      fi_ptr->attr = find_attr_.back();
      FindClose(hfind_.back());

      hfind_.pop_back();
      find_attr_.pop_back();
      if (hfind_.empty() == false) {
        path_.reduce();
      }
      find = true;
    }
  }

  return find;
}

namespace {

template <typename T, typename F>
inline bool compare_4byte(const T& lhs, const F& rhs) {
  return *reinterpret_cast<const uint32_t*>(lhs) ==
    *reinterpret_cast<const uint32_t*>(rhs);
}

}  // namespace

bool file_enumerator::is_period_path(const WIN32_FIND_DATA &wfd) const {
  constexpr wchar_t *CURR_PATH = L".";
  constexpr wchar_t *PARENT_PATH = L"..";

  return compare_4byte(wfd.cFileName, CURR_PATH) ||
    (wfd.cFileName[2] == L'\0' && compare_4byte(wfd.cFileName, PARENT_PATH));
}
