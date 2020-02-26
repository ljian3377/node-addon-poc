#include "transformer.h"
#include <iostream>
#include <cstdio>

Napi::FunctionReference Transformer::constructor;

Napi::Object Transformer::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "Transformer", {
        InstanceMethod("trans", &Transformer::trans)
    });

    // Create a peristent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    constructor = Napi::Persistent(func);
    // Call the SuppressDestruct() method on the static data prevent the calling
    // to this destructor to reset the reference when the environment is no longer
    // available.
    constructor.SuppressDestruct();
    exports.Set("Transformer", func);
    return exports;
}
    
Transformer::Transformer(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Transformer>(info) {
    printf("%d\n", __LINE__);
}

Napi::Value Transformer::trans(const Napi::CallbackInfo &info) {
    printf("%d\n", __LINE__);
    if (info.Length() < 1) {
        throw Napi::Error::New(info.Env(), "1 argument expected");
    }
    if (!info[0].IsBuffer()) {
        throw Napi::Error::New(info.Env(), "The parameter must be a buffer");
    }
    
    Napi::Buffer<char> buf = info[0].As<Napi::Buffer<char>>();
    // buf.Data();
    // buf.ByteLength();

    printf((char*)buf.Data(), buf.ByteLength());
    return buf;
}

Transformer::~Transformer() {
    printf("%d\n", __LINE__);
}
