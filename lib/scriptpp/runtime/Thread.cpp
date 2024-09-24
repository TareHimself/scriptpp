#include "scriptpp/runtime/Thread.hpp"

#include "scriptpp/runtime/Boolean.hpp"
#include "scriptpp/runtime/eval.hpp"
#include "scriptpp/runtime/Exception.hpp"
#include "scriptpp/runtime/Null.hpp"

namespace spp::runtime
{

    Thread::Thread(const std::shared_ptr<ScopeLike>& scope) : DynamicObject(scope)
    {
    }

    std::shared_ptr<ThreadPrototype> Thread::Prototype = makeObject<ThreadPrototype>();

    void Thread::Init()
    {
        DynamicObject::Init();
        AddNativeMemberFunction(ReservedDynamicFunctions::CONSTRUCTOR,this,vectorOf<std::string>("callback"),&Thread::Constructor);
        AddNativeMemberFunction("start",this,vectorOf<std::string>(),&Thread::Start);
        AddNativeMemberFunction("isActive",this,vectorOf<std::string>(),&Thread::IsActive);
        AddNativeMemberFunction("join",this,vectorOf<std::string>(),&Thread::Join);
    }

    std::shared_ptr<Object> Thread::CreateInstance(const std::shared_ptr<FunctionScope>& fnScope)
    {
        auto obj = makeThread({});
        obj->Constructor(fnScope);
        return obj;
    }

    std::shared_ptr<Object> Thread::Start(const std::shared_ptr<FunctionScope>& fnScope)
    {
        if(_thread.joinable()) throw makeException(fnScope,"Cannot start thread that has already been started");
        
        auto myRef = GetRef();
        
        _thread = std::thread([this,myRef]
        {
            _fn.first->Call(_fn.second);
        });
        return makeNull();
    }

    std::shared_ptr<Object> Thread::Join(const std::shared_ptr<FunctionScope>& fnScope)
    {
        if(_thread.joinable())
        {
            _thread.join();
            return makeNull();
        }

        throw makeException(fnScope,"cannot join thread");
    }

    std::shared_ptr<Object> Thread::IsActive(const std::shared_ptr<FunctionScope>& fnScope)
    {
        return makeBoolean(_thread.joinable());
    }

    std::shared_ptr<Object> Thread::Constructor(const std::shared_ptr<FunctionScope>& fnScope)
    {
        auto callback = resolveReference(fnScope->GetArg(0));
        _fn = resolveCallable(callback,fnScope);
        if(!_fn.first) throw makeException(fnScope, "Invalid callback");
        return makeNull();
    }

    std::shared_ptr<Thread> makeThread(const std::shared_ptr<ScopeLike>& scope)
    {
        return makeObject<Thread>(scope);
    }

    ThreadPrototype::ThreadPrototype() : Prototype({},makeNativeFunction(
                                                   {}, ReservedDynamicFunctions::CALL,vectorOf<std::string>(),Thread::CreateInstance))
    {
    }

    std::string ThreadPrototype::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "<Prototype : Thread>";
    }
}
