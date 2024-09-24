#pragma once
#include <thread>

#include "DynamicObject.hpp"
#include "Prototype.hpp"

namespace spp::runtime
{

    class ThreadPrototype;
    
    class Thread : public DynamicObject
    {
        std::thread _thread{};
        
    protected:
        std::pair<std::shared_ptr<Function>,std::shared_ptr<ScopeLike>> _fn{};
    public:
        static std::shared_ptr<ThreadPrototype> Prototype;
        explicit Thread(const std::shared_ptr<ScopeLike>& scope);
        void Init() override;
        static std::shared_ptr<Object> CreateInstance(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Start(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Join(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> IsActive(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Constructor(const std::shared_ptr<FunctionScope>& fnScope);
    };

    std::shared_ptr<Thread> makeThread(const std::shared_ptr<ScopeLike>& scope);

    class ThreadPrototype : public Prototype
    {
    public:
        ThreadPrototype();

        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
    };
}
