#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define WRAPPEDTASK_EXPORT __declspec(dllexport)
#else
  #define WRAPPEDTASK_EXPORT
#endif

WRAPPEDTASK_EXPORT void WrappedTask();
WRAPPEDTASK_EXPORT void WrappedTask_print_vector(const std::vector<std::string> &strings);
