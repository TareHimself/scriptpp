#pragma once
#include <memory>

#include "DynamicObject.hpp"
#include "Function.hpp"
#include "Module.hpp"
#include "Object.hpp"
#include "Prototype.hpp"
#include "Scope.hpp"
#include "vscript/parser/ast.hpp"

namespace vs::runtime
{
    class  EvalError : std::exception
    {

        

        EvalError(uint32_t line,uint32_t col,const std::string& message);
    };
    TSmartPtrType<Object> evalBinaryOperation(const std::shared_ptr<parser::BinaryOpNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> runScope(const std::shared_ptr<parser::ScopeNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalWhen(const std::shared_ptr<parser::WhenNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalExpression(const std::shared_ptr<parser::Node>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Function> evalFunction(const std::shared_ptr<parser::FunctionNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> callFunction(const std::shared_ptr<parser::CallNode>& ast,const TSmartPtrType<Function>& fn,const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalCall(const std::shared_ptr<parser::CallNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalFor(const std::shared_ptr<parser::ForNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalWhile(const std::shared_ptr<parser::WhileNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalStatement(const std::shared_ptr<parser::Node>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalAccess(const std::shared_ptr<parser::AccessNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalAccess2(const std::shared_ptr<parser::AccessNode2>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> evalAssign(const std::shared_ptr<parser::AssignNode>& ast, const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Module> evalModule(const std::shared_ptr<parser::ModuleNode>& ast, const TSmartPtrType<ModuleScope>& scope);
    
    TSmartPtrType<Module> evalModule(const std::shared_ptr<parser::ModuleNode>& ast,const TSmartPtrType<ProgramScope>& program);
    TSmartPtrType<Prototype> evalClass(const std::shared_ptr<parser::PrototypeNode>& ast,const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<DynamicObject> createDynamicFromPrototype(const std::shared_ptr<parser::PrototypeNode>& ast,const TSmartPtrType<ScopeLike>& scope);
    TSmartPtrType<Object> eval(const std::shared_ptr<parser::Node>& ast);
}
