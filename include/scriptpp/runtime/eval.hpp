#pragma once
#include <memory>

#include "DynamicObject.hpp"
#include "Function.hpp"
#include "Module.hpp"
#include "Object.hpp"
#include "Prototype.hpp"
#include "Scope.hpp"
#include "scriptpp/frontend/parser.hpp"

namespace spp::runtime
{
    std::shared_ptr<Object> evalBinaryOperation(const std::shared_ptr<frontend::BinaryOpNode>& ast,
                                                const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> runScope(const std::shared_ptr<frontend::ScopeNode>& ast,
                                     const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalWhen(const std::shared_ptr<frontend::WhenNode>& ast,
                                     const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalExpression(const std::shared_ptr<frontend::Node>& ast,
                                           const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Function> evalFunction(const std::shared_ptr<frontend::FunctionNode>& ast,
                                           const std::shared_ptr<ScopeLike>& scope);
    std::pair<std::shared_ptr<Function>, std::shared_ptr<ScopeLike>> resolveCallable(
        const std::shared_ptr<Object>& target, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> callFunction(const std::shared_ptr<frontend::CallNode>& ast,
                                         const std::shared_ptr<Function>& fn, const std::shared_ptr<ScopeLike>& scope,
                                         const std::shared_ptr<CallScope>& callScope);
    std::shared_ptr<Object> evalCall(const std::shared_ptr<frontend::CallNode>& ast,
                                     const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalFor(const std::shared_ptr<frontend::ForNode>& ast,
                                    const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalWhile(const std::shared_ptr<frontend::WhileNode>& ast,
                                      const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalStatement(const std::shared_ptr<frontend::Node>& ast,
                                          const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalAccess(const std::shared_ptr<frontend::AccessNode>& ast,
                                       const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalIndex(const std::shared_ptr<frontend::IndexNode>& ast,
                                        const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalAssign(const std::shared_ptr<frontend::AssignNode>& ast,
                                       const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalTryCatch(const std::shared_ptr<frontend::TryCatchNode>& ast,
                                         const std::shared_ptr<ScopeLike>& scope);

    std::shared_ptr<Module> evalModule(const std::shared_ptr<frontend::ModuleNode>& ast,
                                       const std::shared_ptr<Program>& program,const std::shared_ptr<ScopeLike>& scope);
    
    std::shared_ptr<Module> evalModule(const std::shared_ptr<frontend::ModuleNode>& ast,
                                       const std::shared_ptr<Program>& program);
    
    std::shared_ptr<Prototype> evalClass(const std::shared_ptr<frontend::PrototypeNode>& ast,
                                         const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<DynamicObject> createDynamicFromPrototype(const std::shared_ptr<frontend::PrototypeNode>& ast,
                                                              const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> eval(const std::shared_ptr<frontend::Node>& ast);
}
