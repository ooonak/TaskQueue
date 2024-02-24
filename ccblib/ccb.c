#define _GNU_SOURCE

#include "ccb/ccb.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  unsigned int sleep_seconds;
  int do_fail;
  ccb_error_cb_t error_cb;
  ccb_result_cb_t result_cb;
  void *context;
  char *input;
} CCB_REQUEST_T;

static CCB_REQUEST_T next_request;
static atomic_bool do_stop = false;
static atomic_bool request_waiting = false;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static const char * const MSG_TEMPL =
    "Everything ended succesfully, off course the result is \"Hello World!\" (your input was \"%s\") :-)";

void ccb_error_example(int error_code, const char *error_descr, void *context)
{
  printf("[%d] %s() %d %s (context: %p)\n", gettid(), __func__, error_code, error_descr, context);
}

void ccb_result_example(char *output, void *context)
{
  printf("[%d] %s() %s (context: %p)\n", gettid(), __func__, (output != NULL) ? output : "NULL", context);

  if (output != NULL) {
    free(output);
  }
}

CCB_ERR_CODE_T request_action_1(const char *input, unsigned int sleep_seconds, bool do_fail, ccb_error_cb_t error_cb,
                                ccb_result_cb_t result_cb, void *context)
{
  if (error_cb == NULL || result_cb == NULL || input == NULL) {
    printf("%s() Invalid input", __func__);
    return CCB_ERROR;
  }

  if (do_stop) {
    return CCB_ERROR;
  }

  while (request_waiting) {
  }

  pthread_mutex_lock(&mutex);
  next_request.sleep_seconds = sleep_seconds;
  next_request.do_fail = do_fail;
  next_request.error_cb = error_cb;
  next_request.result_cb = result_cb;
  next_request.context = context;
  next_request.input = strdup(input);
  request_waiting = true;
  pthread_mutex_unlock(&mutex);

  printf("[%d] %s() Request added to event loop (context: %p)\n", gettid(), __func__, context);

  return CCB_OK;
}

void run()
{
  CCB_REQUEST_T request;

  while (!do_stop) {
    if (request_waiting) {
      pthread_mutex_lock(&mutex);
      request = next_request;
      request.input = strdup(next_request.input);
      free(next_request.input);
      memset(&next_request, 0x0, sizeof(CCB_REQUEST_T));
      pthread_mutex_unlock(&mutex);
      request_waiting = false;

      printf("[%d] %s() About to do long running blocking work (context %p)\n", gettid(), __func__, request.context);

      sleep(request.sleep_seconds);

      if (request.do_fail == true) {
        request.error_cb(42, "The famous 42 error", request.context);
      } else {
        char *msg = NULL;
        size_t length = snprintf(NULL, 0, MSG_TEMPL, request.input);
        if (length != 0) {
          msg = (char *)malloc(sizeof(char) * length);
          if (msg != NULL) {
            snprintf(msg, length, MSG_TEMPL, request.input);
          }
        }

        request.result_cb(msg, request.context);
      }

      printf("[%d] %s() Action completed, done (context: %p)\n", gettid(), __func__, request.context);
    }
  }
}

void stop() { do_stop = true; }
