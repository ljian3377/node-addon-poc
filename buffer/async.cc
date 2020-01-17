#include "async.h"  // NOLINT(build/include)
#include <napi.h>
#include <stdio.h>
#include <list>
#include <chrono>
#include <ctime>
#include <iostream>
using namespace std;

struct Int32Array {
  int32_t* buffer;
  size_t length;

  Int32Array(int32_t* buf, size_t len): buffer(buf), length(len) {}
};

static Napi::Reference<Napi::ArrayBuffer> bufferRef;
static int byteLen = 1024*1024*1024;
static void freeData(Napi::Env env, void* finalizeData) {
  cout << "freeing Data" << endl;
  delete[] static_cast<uint8_t*>(finalizeData);
  cout << __FUNCTION__ << ":ajusted external mem:" << Napi::MemoryManagement::AdjustExternalMemory(env, -byteLen) << endl;
}

class BufferWorker : public Napi::AsyncWorker {
 public:
  BufferWorker(Napi::Function& callback, Int32Array arr)
      : Napi::AsyncWorker(callback), array(arr) {}
  ~BufferWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access JS engine data structure
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() {
    array.buffer[0] = 333;
    return;
  }

  void OnOK() {
    bufferRef.Unref();
    uint8_t* data = nullptr;
    cout << "allocate Data" << endl;
    try {
      data = new uint8_t[byteLen];
    } catch (...) {
      std::cout << "exception occured" << std::endl;
    }
    cout << __FUNCTION__ << ":ajusted external mem:" << Napi::MemoryManagement::AdjustExternalMemory(Env(), byteLen) << endl;
    auto buffer = Napi::ArrayBuffer::New(Env(), data, byteLen, freeData);
    Callback().Call({buffer});
  }

  Int32Array& GetArray() {
    return array;
  }

 private:
  Int32Array array;
};


static char* Now() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  return std::ctime(&now_time);
}

static list<BufferWorker*>  workerQueue;
// Asynchronous access to the `Estimate()` function
Napi::Value CalculateAsync(const Napi::CallbackInfo& info) {
  if (info.Length() != 2) {
    Napi::Error::New(info.Env(), "Expected exactly one argument")
        .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  if (!info[0].IsArrayBuffer()) {
    Napi::Error::New(info.Env(), "Expected an ArrayBuffer")
        .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  Napi::ArrayBuffer buf = info[0].As<Napi::ArrayBuffer>();
  
  bufferRef = Napi::Reference<Napi::ArrayBuffer>::New(buf, 1);
  bufferRef.SuppressDestruct();
  
  Int32Array array(reinterpret_cast<int32_t*>(buf.Data()), buf.ByteLength() / sizeof(int32_t));
  Napi::Function callback = info[1].As<Napi::Function>();
  BufferWorker* bufferWorker = new BufferWorker(callback, array);
  workerQueue.push_back(bufferWorker);
  cout << "work pushed at " << Now() << endl;
  return info.Env().Undefined();
}


static int i = 0;
void QueueWork() {
  cout << __FUNCTION__ << ":" << ++i << endl;
  if (!workerQueue.empty()) {
    auto &array = (workerQueue.front())->GetArray();
    // std::cout << "array.length: " << array.length << ", array[last]: " << array.buffer[array.length -1] << std::endl;
    workerQueue.front()->Queue();
    // std::cout << "work queued to libuv at "<< Now() << std::endl;
    workerQueue.pop_front();
  }
  return;
}