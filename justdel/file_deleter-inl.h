// Copyright 2017 jeonghun

#ifndef JUSTDEL_FILE_DELETER_INL_H_
#define JUSTDEL_FILE_DELETER_INL_H_

#include "file_deleter.h"

bool file_deleter::erase(const file_enumerator::info_t &fi) {
  bool erase = false;

  if (fi.attr & FILE_ATTRIBUTE_READONLY) {
    SetFileAttributes(fi.path, fi.attr & ~FILE_ATTRIBUTE_READONLY);
  }
  erase = ((fi.attr & FILE_ATTRIBUTE_DIRECTORY) ?
    RemoveDirectory(fi.path) : DeleteFile(fi.path)) != FALSE;
  erase ? ++num_of_deleted_ : ++num_of_failed_;

  return erase;
}

#endif  // JUSTDEL_FILE_DELETER_INL_H_
