// Copyright 2017 jeonghun

#ifndef JUSTDEL_FILE_DELETER_H_
#define JUSTDEL_FILE_DELETER_H_

#include "file_enumerator.h"

class file_deleter final {
 public:
  file_deleter() = default;
  ~file_deleter() = default;

  inline bool erase(const file_enumerator::info_t &fi);
  inline size_t get_num_of_deleted() const { return num_of_deleted_; }
  inline size_t get_num_of_failed() const { return num_of_failed_; }

 private:
  size_t num_of_deleted_ = 0;
  size_t num_of_failed_ = 0;
};

#include "file_deleter-inl.h"

#endif  // JUSTDEL_FILE_DELETER_H_
