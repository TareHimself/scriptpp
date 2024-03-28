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
    
    TSmartPtrType<Object> evalBinaryOperation(const std::shared_ptr<backend::BinaryOpNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> runScope(const std::shared_ptr<backend::ScopeNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalWhen(const std::shared_ptr<backend::WhenNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalExpression(const std::shared_ptr<backend::Node>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Function> evalFunction(const std::shared_ptr<backend::FunctionNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> callFunction(const std::shared_ptr<backend::CallNode>& ast,const TSmartPtrType<Function>& fn,const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalCall(const std::shared_ptr<backend::CallNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalFor(const std::shared_ptr<backend::ForNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalWhile(const std::shared_ptr<backend::WhileNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalStatement(const std::shared_ptr<backend::Node>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalAccess(const std::shared_ptr<backend::AccessNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalAccess2(const std::shared_ptr<backend::AccessNode2>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalAssign(const std::shared_ptr<backend::AssignNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalTryCatch(const std::shared_ptr<backend::TryCatchNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Module> evalModule(const std::shared_ptr<backend::ModuleNode>& ast, const TSmartPtrType<Program>& scope);
    TSmartPtrType<Prototype> evalClass(const std::shared_ptr<backend::PrototypeNode>& ast,const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<DynamicObject> createDynamicFromPrototype(const std::shared_ptr<backend::PrototypeNode>& ast,const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> eval(const std::shared_ptr<backend::Node>& ast);
}
