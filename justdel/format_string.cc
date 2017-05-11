// Copyright 2016 jeonghun

#include "format_string.h"
#include <cstdint>
#include <cstdarg>

namespace {

inline int vsprintfT(char *buffer, size_t buf_size, const char *format, va_list argptr) {
  return vsprintf_s(buffer, buf_size, format, argptr);
}

inline int vsprintfT(wchar_t *buffer, size_t buf_size, const wchar_t *format, va_list argptr) {
  return vswprintf_s(buffer, buf_size, format, argptr);
}

inline int vscprintfT(const char *format, va_list argptr) {
  return _vscprintf(format, argptr);
}

inline int vscprintfT(const wchar_t *format, va_list argptr) {
  return _vscwprintf(format, argptr);
}

template <typename T>
inline void printf_like_format(T &output, const typename T::value_type *format, va_list argptr) {
  if (format != nullptr && argptr != nullptr) {
    output.resize(vscprintfT(format, argptr));
    vsprintfT(&output.at(0), output.size() + 1, format, argptr);
  }
}

}  // namespace

std::string format_string(const char* format, ...) {
  std::string result;

  va_list argptr;
  va_start(argptr, format);
  printf_like_format(result, format, argptr);
  va_end(argptr);

  return result;
}

std::wstring format_string(const wchar_t* format, ...) {
  std::wstring result;

  va_list argptr;
  va_start(argptr, format);
  printf_like_format(result, format, argptr);
  va_end(argptr);

  return result;
}
