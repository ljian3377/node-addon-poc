#include <napi.h>
#include "transformer.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Transformer::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)