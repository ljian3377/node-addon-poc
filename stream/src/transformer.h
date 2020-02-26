#include <napi.h>

class Transformer : public Napi::ObjectWrap<Transformer> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Transformer(const Napi::CallbackInfo &info);
    ~Transformer();
    
    Napi::Value trans(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference constructor;
};
