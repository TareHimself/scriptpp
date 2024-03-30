#pragma once
#include <memory>

#include "DynamicObject.hpp"
#include "Function.hpp"
#include "Module.hpp"
#include "Object.hpp"
#include "Prototype.hpp"
#include "Scope.hpp"
#include "vscript/backend/ast.hpp"

namespace vs::frontend
{
    class  EvalError : std::exception
    {
        EvalError(uint32_t line,uint32_t col,const std::string& message);
    };
    
    std::shared_ptr<Object> evalBinaryOperation(const std::shared_ptr<backend::BinaryOpNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> runScope(const std::shared_ptr<backend::ScopeNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalWhen(const std::shared_ptr<backend::WhenNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalExpression(const std::shared_ptr<backend::Node>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Function> evalFunction(const std::shared_ptr<backend::FunctionNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> callFunction(const std::shared_ptr<backend::CallNode>& ast,const std::shared_ptr<Function>& fn,const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<CallScope>& callScope);
    std::shared_ptr<Object> evalCall(const std::shared_ptr<backend::CallNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalFor(const std::shared_ptr<backend::ForNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalWhile(const std::shared_ptr<backend::WhileNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalStatement(const std::shared_ptr<backend::Node>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalAccess(const std::shared_ptr<backend::AccessNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalAccess2(const std::shared_ptr<backend::AccessNode2>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalAssign(const std::shared_ptr<backend::AssignNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> evalTryCatch(const std::shared_ptr<backend::TryCatchNode>& ast, const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Module> evalModule(const std::shared_ptr<backend::ModuleNode>& ast, const std::shared_ptr<Program>& scope);
    std::shared_ptr<Prototype> evalClass(const std::shared_ptr<backend::PrototypeNode>& ast,const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<DynamicObject> createDynamicFromPrototype(const std::shared_ptr<backend::PrototypeNode>& ast,const std::shared_ptr<ScopeLike>& scope);
    std::shared_ptr<Object> eval(const std::shared_ptr<backend::Node>& ast);
}
