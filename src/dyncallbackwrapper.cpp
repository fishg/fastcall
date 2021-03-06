/*
Copyright 2016 Gábor Mező (gabor.mezo@outlook.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "dyncallbackwrapper.h"
#include "callbackimpl.h"
#include "dcbarg.h"
#include "deps.h"
#include "getv8value.h"
#include "helpers.h"
#include "int64.h"
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

NAN_METHOD(makePtr)
{
    auto callback = info[0].As<Object>();
    auto loop = Unwrap<Loop>(info[1]);
    auto signature = string(*Nan::Utf8String(info[2]));
    assert(signature.size() >= 2);
    auto execute = info[3].As<Function>();
    auto func = info[4].As<Function>();

    auto userData = new CallbackUserData(
        signature[signature.size() - 1],
        Nan::Global<Function>(execute),
        Nan::Global<Function>(func),
        loop);
    auto dcCallback = MakeDCCallback(signature, userData);

    auto userDataPtr = Wrap(userData);
    auto dcCallbackPtr = Wrap(dcCallback, [](char* data, void* hint) { dcbFreeCallback(reinterpret_cast<DCCallback*>(data)); });
    SetValue(dcCallbackPtr, "userData", userDataPtr);
    SetValue(dcCallbackPtr, "callback", callback);

    info.GetReturnValue().Set(dcCallbackPtr);
}

NAN_METHOD(argBool)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgBoolean(args)));
}

NAN_METHOD(argChar)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgChar(args)));
}

NAN_METHOD(argShort)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgShort(args)));
}

NAN_METHOD(argInt)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgInt(args)));
}

NAN_METHOD(argLong)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(MakeInt64(dcbArgLong(args)));
}

NAN_METHOD(argLongLong)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(MakeInt64(dcbArgLongLong(args)));
}

NAN_METHOD(argFloat)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgFloat(args)));
}

NAN_METHOD(argDouble)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgDouble(args)));
}

NAN_METHOD(argPointer)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(WrapPointer(dcbArgPointer(args)));
}

NAN_METHOD(argInt8)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgInt8(args)));
}

NAN_METHOD(argUInt8)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgUInt8(args)));
}

NAN_METHOD(argInt16)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgInt16(args)));
}

NAN_METHOD(argUInt16)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgUInt16(args)));
}

NAN_METHOD(argInt32)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgInt32(args)));
}

NAN_METHOD(argUInt32)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgUInt32(args)));
}

NAN_METHOD(argInt64)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(MakeInt64(dcbArgInt64(args)));
}

NAN_METHOD(argUInt64)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(MakeUint64(dcbArgUInt64(args)));
}

NAN_METHOD(argByte)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgByte(args)));
}

NAN_METHOD(argUChar)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgUChar(args)));
}

NAN_METHOD(argUShort)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgUShort(args)));
}

NAN_METHOD(argUInt)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(Nan::New(dcbArgUInt(args)));
}

NAN_METHOD(argULong)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(MakeUint64(dcbArgULong(args)));
}

NAN_METHOD(argULongLong)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(MakeUint64(dcbArgULongLong(args)));
}

NAN_METHOD(argSizeT)
{
    auto args = Unwrap<DCArgs>(info[0]);
    info.GetReturnValue().Set(MakeUint64(dcbArgSizeT(args)));
}

NAN_METHOD(setPointer)
{
    auto value = Unwrap<DCValue>(info[0]);
    try {
        value->p = GetPointer(info[1]);
    } catch (exception& ex) {
        Nan::ThrowTypeError(ex.what());
    }
}

NAN_METHOD(setInt8)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->c = GetInt8(info[1]);
}

NAN_METHOD(setUInt8)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->C = GetUInt8(info[1]);
}

NAN_METHOD(setInt16)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->s = GetInt16(info[1]);
}

NAN_METHOD(setUInt16)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->S = GetUInt16(info[1]);
}

NAN_METHOD(setInt32)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->i = GetInt32(info[1]);
}

NAN_METHOD(setUInt32)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->I = GetUInt32(info[1]);
}

NAN_METHOD(setInt64)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->l = GetInt64(info[1]);
}

NAN_METHOD(setUInt64)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->L = GetUint64(info[1]);
}

NAN_METHOD(setFloat)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->f = GetFloat(info[1]);
}

NAN_METHOD(setDouble)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->d = GetDouble(info[1]);
}

NAN_METHOD(setChar)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->c = GetChar(info[1]);
}

NAN_METHOD(setByte)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->C = GetByte(info[1]);
}

NAN_METHOD(setUChar)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->C = GetUChar(info[1]);
}

NAN_METHOD(setShort)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->s = GetShort(info[1]);
}

NAN_METHOD(setUShort)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->S = GetUShort(info[1]);
}

NAN_METHOD(setInt)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->i = GetInt(info[1]);
}

NAN_METHOD(setUInt)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->I = GetUInt(info[1]);
}

NAN_METHOD(setLong)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->j = GetLong(info[1]);
}

NAN_METHOD(setULong)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->J = GetULong(info[1]);
}

NAN_METHOD(setLongLong)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->l = GetLongLong(info[1]);
}

NAN_METHOD(setULongLong)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->L = GetULongLong(info[1]);
}

NAN_METHOD(setBool)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->C = GetBool(info[1]);
}

NAN_METHOD(setSizeT)
{
    auto value = Unwrap<DCValue>(info[0]);
    value->J = GetSizeT(info[1]);
}
}

NAN_MODULE_INIT(fastcall::InitCallbackWrapper)
{
    auto callback = Nan::New<Object>();
    Nan::Set(target, Nan::New<String>("callback").ToLocalChecked(), callback);
    Nan::Set(callback, Nan::New<String>("newLoop").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(newLoop)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("freeLoop").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(freeLoop)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("makePtr").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(makePtr)).ToLocalChecked());

    Nan::Set(callback, Nan::New<String>("argBool").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argBool)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argChar").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argChar)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argShort").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argShort)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argInt").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argInt)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argLong").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argLong)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argLongLong").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argLongLong)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argFloat").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argFloat)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argDouble").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argDouble)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argPointer").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argPointer)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argInt8").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argInt8)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argUInt8").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argUInt8)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argInt16").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argInt16)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argUInt16").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argUInt16)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argInt32").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argInt32)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argUInt32").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argUInt32)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argInt64").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argInt64)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argUInt64").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argUInt64)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argByte").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argByte)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argUChar").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argUChar)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argUShort").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argUShort)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argUInt").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argUInt)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argULong").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argULong)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argULongLong").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argULongLong)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("argSizeT").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(argSizeT)).ToLocalChecked());

    Nan::Set(callback, Nan::New<String>("setBool").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setBool)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setChar").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setChar)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setShort").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setShort)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setInt").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setInt)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setLong").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setLong)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setLongLong").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setLongLong)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setFloat").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setFloat)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setDouble").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setDouble)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setPointer").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setPointer)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setInt8").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setInt8)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setInt16").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setInt16)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setInt32").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setInt32)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setInt64").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setInt64)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setUInt8").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setUInt8)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setUInt16").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setUInt16)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setUInt32").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setUInt32)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setUInt64").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setUInt64)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setByte").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setByte)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setUChar").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setUChar)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setUShort").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setUShort)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setUInt").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setUInt)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setULong").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setULong)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setULongLong").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setULongLong)).ToLocalChecked());
    Nan::Set(callback, Nan::New<String>("setSizeT").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setSizeT)).ToLocalChecked());
}
