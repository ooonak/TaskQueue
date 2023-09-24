# TaskQueue
## Idea
Just playing around, learning stuff, creating a task queue to push jobs across threads. 
It should work like std::async, but tasks should be placed in a queue, so a specific worker thread can consume them later.

 - Make it work like std::async, acceptiong any function.
 - std::bind stores a copy of the bound arguments.

## Build & run
```bash
$ cmake -G Ninja -S . -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
$ cmake --build build
$ cd build
$ ctest
$ ./example/TaskQueueExample
```

## References
  - C++ Concurrency in action, second edition, p. 86.
  - https://en.cppreference.com/w/cpp/thread/async
  - https://en.cppreference.com/w/cpp/types/result_of
  - https://stackoverflow.com/questions/5889532/how-do-i-create-a-queue-that-holds-boostpackaged-task-with-functions-that-re
  - https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h
