#include <napi.h>
#include "async.h"  // NOLINT(build/include)
#include <thread>
#include <chrono>

void loop_task() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    QueueWork();
  }
}

// Expose synchronous and asynchronous access to our
// Estimate() function
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "calculateAsync"),
              Napi::Function::New(env, CalculateAsync));
  
  std::thread loopThread(loop_task);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
