#include "async.h"  // NOLINT(build/include)
#include <napi.h>
#include "pi_est.h"  // NOLINT(build/include)

class PiWorker : public Napi::AsyncWorker {
 public:
  PiWorker(Napi::Function& callback, int points)
      : Napi::AsyncWorker(callback), points(points), estimate(0) {}
  ~PiWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access JS engine data structure
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() { estimate = Estimate(points); }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use JS engine data again
  void OnOK() {
    Napi::HandleScope scope(Env());
    Callback().Call({Env().Undefined(), Napi::Number::New(Env(), estimate)});
  }

 private:
  int points;
  double estimate;
};

#include <list>
#include <chrono>
#include <ctime>
#include <iostream>
using namespace std;

static char* Now() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  return std::ctime(&now_time);
}

static list<PiWorker*>  workerQueue;
// Asynchronous access to the `Estimate()` function
Napi::Value CalculateAsync(const Napi::CallbackInfo& info) {
  int points = info[0].As<Napi::Number>().Uint32Value();
  Napi::Function callback = info[1].As<Napi::Function>();
  PiWorker* piWorker = new PiWorker(callback, points);
  workerQueue.push_back(piWorker);
  cout << "work pushed at " << Now() << endl;
  return info.Env().Undefined();
}



void QueueWork() {
  cout << __FUNCTION__ << ":" << __LINE__ << endl;
  if (!workerQueue.empty()) {
    workerQueue.front()->Queue();
    std::cout << "work queued to libuv at "<< Now() << std::endl;
    workerQueue.pop_front();
  }
  return;
}