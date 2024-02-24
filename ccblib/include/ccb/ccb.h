#ifndef CCB_H
#define CCB_H

#include <stdbool.h>

#ifdef __CC_PLUS
extern C
{
#endif // __CC_PLUS

  typedef enum { CCB_OK = 0x0, CCB_ERROR } CCB_ERR_CODE_T;

  typedef void (*ccb_error_cb_t)(int error_code, const char *error_descr, void *context);
  typedef void (*ccb_result_cb_t)(char *output, void *context);

  void ccb_error_example(int error_code, const char *error_descr, void *context);

  void ccb_result_example(char *output, void *context);

  CCB_ERR_CODE_T request_action_1(const char *input, unsigned int sleep_seconds, bool do_fail, ccb_error_cb_t error_cb,
                                  ccb_result_cb_t result_cb, void *context);

  void run();

  void stop();

#ifdef __CC_PLUS
}
#endif // __CC_PLUS
       //
#endif // CCB_H
