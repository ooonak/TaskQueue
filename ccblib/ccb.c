#define _GNU_SOURCE

#include "ccb/ccb.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char *MSG_TEMPL =
    "Everything ended succesfully, off course the result is \"Hello World!\" (your input was \"%s\") :-)";

void ccb_error_example(int error_code, const char *error_descr, void *context)
{
  fprintf(stderr, "[%d] %s() %d %s %p\n", gettid(), __func__, error_code, error_descr, context);
}

void ccb_result_example(char *output, void *context)
{
  printf("[%d] %s() %s %p\n", gettid(), __func__, output ? output : "NULL", context);

  if (output != NULL) {
    free(output);
  }
}

CCB_ERR_CODE_T request_action_1(const char *input, unsigned int sleep_seconds, int do_fail, ccb_error_cb_t error_cb,
                                ccb_result_cb_t result_cb, void *context)
{
  if (error_cb == NULL || result_cb == NULL || input == NULL) {
    fprintf(stderr, "%s() Invalid input", __func__);
    return CCB_ERROR;
  }

  printf("[%d] %s() About to do long running blocking work %p\n", gettid(), __func__, context);

  sleep(sleep_seconds);

  if (do_fail != 0) {
    error_cb(42, "The famous 42 error", context);
  } else {
    size_t length = snprintf(NULL, 0, MSG_TEMPL, input);
    if (length == 0) {
      return CCB_ERROR;
    }

    char *msg = (char *)malloc(sizeof(char) * length);
    if (msg == NULL) {
      return CCB_ERROR;
    }

    snprintf(msg, length, MSG_TEMPL, input);

    result_cb(msg, context);
  }

  printf("[%d] %s() Action completed, done %p\n", gettid(), __func__, context);

  return CCB_OK;
}
