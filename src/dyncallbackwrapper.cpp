#include "deps.h"
#include "dyncallbackwrapper.h"
#include "helpers.h"
#include "loop.h"

using namespace std;
using namespace v8;
using namespace node;
using namespace fastcall;

namespace {
NAN_METHOD(newLoop)
{
    info.GetReturnValue().Set(WrapPointer(new Loop()));
}

NAN_METHOD(freeLoop)
{
    delete Unwrap<Loop>(info[0]);
}
}

NAN_MODULE_INIT(fastcall::InitCallbackWrapper)
{
    Nan::HandleScope scope;
    auto callback = Nan::New<Object>();
    Nan::Set(target, Nan::New<String>("callback").ToLocalChecked(), callback);
    Nan::Set(callback, Nan::New<String>("newLoop").ToLocalChecked(), Nan::New<FunctionTemplate>(newLoop)->GetFunction());
    Nan::Set(callback, Nan::New<String>("freeLoop").ToLocalChecked(), Nan::New<FunctionTemplate>(freeLoop)->GetFunction());
}
