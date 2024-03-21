#include "vscript/runtime/eval.hpp"

#include <stdexcept>

#include "vscript/runtime/Boolean.hpp"
#include "vscript/runtime/List.hpp"
#include "vscript/runtime/Null.hpp"
#include "vscript/runtime/Number.hpp"
#include "vscript/runtime/Prototype.hpp"
#include "vscript/runtime/String.hpp"

namespace vs::runtime
{
    TSmartPtrType<Object> evalBinaryOperation(const std::shared_ptr<parser::BinaryOpNode>& ast,
                                              const TSmartPtrType<ScopeLike>& scope)
    {
        switch (ast->op)
        {
        case parser::BinaryOpNode::BO_Divide:
            return evalExpression(ast->left, scope)->Divide(evalExpression(ast->right, scope));
        case parser::BinaryOpNode::BO_Multiply:
            return evalExpression(ast->left, scope)->Multiply(evalExpression(ast->right, scope));
        case parser::BinaryOpNode::BO_Add:
            return evalExpression(ast->left, scope)->Add(evalExpression(ast->right, scope));
        case parser::BinaryOpNode::BO_Subtract:
            return evalExpression(ast->left, scope)->Subtract(evalExpression(ast->right, scope));
        case parser::BinaryOpNode::BO_Mod:
            return evalExpression(ast->left, scope)->Subtract(evalExpression(ast->right, scope));
        case parser::BinaryOpNode::BO_And:
            return makeBoolean(
                evalExpression(ast->left, scope)->ToBoolean() && evalExpression(ast->right, scope)->ToBoolean());
        case parser::BinaryOpNode::BO_Or:
            return makeBoolean(
                evalExpression(ast->left, scope)->ToBoolean() || evalExpression(ast->right, scope)->ToBoolean());
        case parser::BinaryOpNode::BO_Not:
            throw std::runtime_error("This need work");
        case parser::BinaryOpNode::BO_Equal:
            return makeBoolean(evalExpression(ast->left, scope)->Equal(evalExpression(ast->right, scope)));
        case parser::BinaryOpNode::BO_NotEqual:
            return makeBoolean(evalExpression(ast->left, scope)->NotEqual(evalExpression(ast->right, scope)));
        case parser::BinaryOpNode::BO_Less:
            return makeBoolean(evalExpression(ast->left, scope)->Less(evalExpression(ast->right, scope)));
        case parser::BinaryOpNode::BO_LessEqual:
            return makeBoolean(evalExpression(ast->left, scope)->LessEqual(evalExpression(ast->right, scope)));
        case parser::BinaryOpNode::BO_Greater:
            return makeBoolean(evalExpression(ast->left, scope)->Greater(evalExpression(ast->right, scope)));
        case parser::BinaryOpNode::BO_GreaterEqual:
            return makeBoolean(evalExpression(ast->left, scope)->GreaterEqual(evalExpression(ast->right, scope)));
        }
    }

    TSmartPtrType<Object> runScope(const std::shared_ptr<parser::ScopeNode>& ast, const TSmartPtrType<ScopeLike>& scope)
    {
        TSmartPtrType<Object> lastResult = makeNull();

        for (const auto& statement : ast->statements)
        {
            if (auto evalResult = evalStatement(statement, scope); evalResult.IsValid())
            {
                if (evalResult->GetType() == OT_ReturnValue)
                {
                    if (scope->GetScopeType() == ST_Function)
                    {
                        return evalResult.Cast<ReturnValue>()->GetValue();
                    }

                    return evalResult;
                }

                lastResult = evalResult;
            }
        }

        return lastResult;
    }

    TSmartPtrType<Object> evalWhen(const std::shared_ptr<parser::WhenNode>& ast, const TSmartPtrType<ScopeLike>& scope)
    {
        for (auto& branch : ast->branches)
        {
            if (evalExpression(branch.expression, scope)->ToBoolean())
            {
                return evalStatement(branch.statement, scope);
            }
        }

        return makeNull();
    }

    TSmartPtrType<Object> evalExpression(const std::shared_ptr<parser::Node>& ast,
                                         const TSmartPtrType<ScopeLike>& scope)
    {
        switch (ast->type)
        {
        case parser::NT_Variable:
            {
                if (const auto r = std::dynamic_pointer_cast<parser::VariableNode>(ast))
                {
                    if (r->value == "List")
                    {
                        return List::Prototype;
                    }
                    return scope->Find(r->value);
                }
                throw std::runtime_error("Expected variable");
            }
        case parser::NT_StringLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<parser::LiteralNode>(ast))
                {
                    return makeString(r->value);
                }
                throw std::runtime_error("Expected string literal");
            }
        case parser::NT_NumericLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<parser::LiteralNode>(ast))
                {
                    return makeNumber(r->value);
                }
                throw std::runtime_error("Expected numeric literal");
            }
        case parser::NT_BooleanLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<parser::LiteralNode>(ast))
                {
                    return makeBoolean(r->value);
                }
                throw std::runtime_error("Expected boolean literal");
            }
        case parser::NT_BinaryOp:
            {
                if (const auto r = std::dynamic_pointer_cast<parser::BinaryOpNode>(ast))
                {
                    return evalBinaryOperation(r, scope);
                }
                throw std::runtime_error("Expected binary operation");
            }
        case parser::NT_When:
            if (const auto r = std::dynamic_pointer_cast<parser::WhenNode>(ast))
            {
                return evalWhen(r, scope);
            }
        case parser::NT_Assign:
            if (const auto r = std::dynamic_pointer_cast<parser::AssignNode>(ast))
            {
                return evalAssign(r, scope);
            }
        case parser::NT_Function:
            if (const auto r = std::dynamic_pointer_cast<parser::FunctionNode>(ast))
            {
                return evalFunction(r, scope);
            }
        case parser::NT_Call:
            if (const auto r = std::dynamic_pointer_cast<parser::CallNode>(ast))
            {
                return evalCall(r, scope);
            }
        case parser::NT_Access:
            if (const auto r = std::dynamic_pointer_cast<parser::AccessNode>(ast))
            {
                return evalAccess(r, scope);
            }
        case parser::NT_Access2:
            if (const auto r = std::dynamic_pointer_cast<parser::AccessNode2>(ast))
            {
                return evalAccess2(r, scope);
            }
        default:
            throw std::runtime_error("Unknown expression");
        }
    }

    TSmartPtrType<Function> evalFunction(const std::shared_ptr<parser::FunctionNode>& ast,
                                         const TSmartPtrType<ScopeLike>& scope)
    {
        return makeRuntimeFunction(scope, ast->name, ast->args, ast->body);
    }

    TSmartPtrType<Object> callFunction(const std::shared_ptr<parser::CallNode>& ast, const TSmartPtrType<Function>& fn,
                                       const TSmartPtrType<ScopeLike>& scope)
    {
        std::vector<TSmartPtrType<Object>> args;
        for (auto& arg : ast->args)
        {
            args.push_back(evalExpression(arg, scope));
        }

        return fn->Call(args);
    }

    TSmartPtrType<Object> evalCall(const std::shared_ptr<parser::CallNode>& ast, const TSmartPtrType<ScopeLike>& scope)
    {
        if (auto obj = evalExpression(ast->left, scope); obj.IsValid())
        {
            if (obj->GetType() == OT_Function)
            {
                if (const auto asCall = obj.Cast<Function>(); asCall.IsValid())
                {
                    return callFunction(ast, asCall, scope);
                }
            }

            if (obj->GetType() == OT_Dynamic)
            {
                if (const auto asDynamic = obj.Cast<DynamicObject>(); asDynamic.IsValid())
                {
                    if (asDynamic->Has(ReservedDynamicFunctions::CALL))
                    {
                        if (const auto asCall = asDynamic->GetRef(ReservedDynamicFunctions::CALL).Cast<Function>();
                            asCall.IsValid())
                        {
                            return callFunction(ast, asCall, scope);
                        }
                    }
                }
            }

            throw std::runtime_error(obj->ToString() + " is not callable");
        }

        throw std::runtime_error("Unknown object during call");
    }

    TSmartPtrType<Object> evalFor(const std::shared_ptr<parser::ForNode>& ast, const TSmartPtrType<ScopeLike>& scope)
    {
        TSmartPtrType<Object> result = makeNull();
        evalStatement(ast->init, scope);
        while (evalStatement(ast->condition, scope)->ToBoolean())
        {
            if (auto temp = runScope(ast->body, scope); temp.IsValid())
            {
                if (temp->GetType() == OT_ReturnValue)
                {
                    if (scope->HasScopeType(ST_Function))
                    {
                        return temp;
                    }
                }
                else if (temp->GetType() == OT_FlowControl)
                {
                    if (const auto flow = temp.Cast<FlowControl>(); temp.IsValid())
                    {
                        if (flow->GetValue() == FlowControl::Break)
                        {
                            break;
                        }
                        else if (flow->GetValue() == FlowControl::Continue)
                        {
                            continue;
                        }
                    }
                }

                result = temp;
            }

            evalStatement(ast->update, scope);
        }

        return result;
    }

    TSmartPtrType<Object> evalWhile(const std::shared_ptr<parser::WhileNode>& ast,
                                    const TSmartPtrType<ScopeLike>& scope)
    {
        TSmartPtrType<Object> result = makeNull();
        while (evalStatement(ast->condition, scope)->ToBoolean())
        {
            if (auto temp = runScope(ast->body, scope); temp.IsValid())
            {
                if (temp->GetType() == OT_ReturnValue)
                {
                    if (scope->HasScopeType(ST_Function))
                    {
                        return temp;
                    }
                }
                else if (temp->GetType() == OT_FlowControl)
                {
                    if (const auto flow = temp.Cast<FlowControl>(); temp.IsValid())
                    {
                        if (flow->GetValue() == FlowControl::Break)
                        {
                            break;
                        }
                        else if (flow->GetValue() == FlowControl::Continue)
                        {
                            continue;
                        }
                    }
                }

                result = temp;
            }
        }

        return result;
    }

    TSmartPtrType<Object> evalStatement(const std::shared_ptr<parser::Node>& ast, const TSmartPtrType<ScopeLike>& scope)
    {
        switch (ast->type)
        {
        case parser::NT_CreateAndAssign:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::CreateAndAssignNode>(ast))
                {
                    auto result = evalExpression(a->value, scope);
                    scope->Assign(a->name, result);
                    return result;
                }
            }
            break;
        case parser::NT_Function:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::FunctionNode>(ast))
                {
                    auto result = evalFunction(a, scope);
                    if (!a->name.empty())
                    {
                        scope->Assign(a->name, result);
                    }
                    return result;
                }
            }
            break;
        case parser::NT_Call:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::CallNode>(ast))
                {
                    return evalCall(a, scope);
                }
            }
            break;
        case parser::NT_Scope:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::ScopeNode>(ast))
                {
                    return runScope(a, scope);
                }
            }
            break;
        case parser::NT_Return:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::ReturnNode>(ast))
                {
                    if (scope->HasScopeType(ST_Function))
                    {
                        return makeReturnValue(evalExpression(a->expression, scope));
                    }
                    return evalExpression(a->expression, scope);
                }
            }
            break;
        case parser::NT_When:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::WhenNode>(ast))
                {
                    return evalWhen(a, scope);
                }
            }
            break;
        case parser::NT_For:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::ForNode>(ast))
                {
                    return evalFor(a, scope);
                }
            }
            break;
        case parser::NT_While:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::WhileNode>(ast))
                {
                    return evalWhile(a, scope);
                }
            }
            break;
        case parser::NT_Break:
            return makeFlowControl(FlowControl::Break);
        case parser::NT_Continue:
            return makeFlowControl(FlowControl::Continue);
        case parser::NT_Class:
            {
                if (const auto a = std::dynamic_pointer_cast<parser::PrototypeNode>(ast))
                {
                    return evalClass(a, scope);
                }
            }
            break;
        default:
            return evalExpression(ast, scope);
        }
    }

    TSmartPtrType<Object> evalAccess(const std::shared_ptr<parser::AccessNode>& ast,
                                     const TSmartPtrType<ScopeLike>& scope)
    {
        auto target = evalExpression(ast->left, scope);
        if (target->GetType() != OT_Dynamic)
        {
            throw std::runtime_error(target->ToString() + " is not a dynamic object");
        }

        return evalExpression(ast->right, target.Cast<DynamicObject>()->GetScope());
    }

    TSmartPtrType<Object> evalAccess2(const std::shared_ptr<parser::AccessNode2>& ast,
                                      const TSmartPtrType<ScopeLike>& scope)
    {
        auto target = evalExpression(ast->left, scope);

        if (target->GetType() == OT_Dynamic)
        {
            const auto within = evalExpression(ast->within, scope);

            if (auto dyn = target.Cast<DynamicObject>(); dyn.IsValid())
            {
                return dyn->Get(within);
            }
        }

        throw std::runtime_error(target->ToString() + " is not a dynamic object");
    }

    TSmartPtrType<Object> evalAssign(const std::shared_ptr<parser::AssignNode>& ast,
                                     const TSmartPtrType<ScopeLike>& scope)
    {
        if (ast->left->type == parser::NT_Access)
        {
            if (const auto accessNode = std::dynamic_pointer_cast<parser::AccessNode>(ast->left))
            {
                if (accessNode->right->type == parser::NT_Variable)
                {
                    if (auto accessed = evalExpression(accessNode->left, scope).Cast<DynamicObject>(); accessed.
                        IsValid())
                    {
                        const auto var = std::dynamic_pointer_cast<parser::VariableNode>(accessNode->right);
                        auto val = evalExpression(ast->value, scope);
                        accessed->GetScope()->Assign(var->value, val);
                        return val;
                    }
                }
            }
        }
        else if (ast->left->type == parser::NT_Access2)
        {
            if (const auto accessNode = std::dynamic_pointer_cast<parser::AccessNode2>(ast->left))
            {
                if (auto target = evalExpression(accessNode->left, scope); target->GetType() == OT_Dynamic)
                {
                    if (auto dyn = target.Cast<DynamicObject>(); dyn.IsValid())
                    {
                        auto val = evalExpression(ast->value, scope);
                        dyn->Set(evalExpression(accessNode->within, scope), val);
                        return val;
                    }
                }
            }
        }
        else if (ast->left->type == parser::NT_Variable)
        {
            const auto var = std::dynamic_pointer_cast<parser::VariableNode>(ast->left);
            auto val = evalExpression(ast->value, scope);
            scope->Assign(var->value, val);
            return val;
        }

        throw std::runtime_error("Assign failed");
    }

    TSmartPtrType<Module> evalModule(const std::shared_ptr<parser::ModuleNode>& ast,
                                     const TSmartPtrType<ModuleScope>& scope)
    {
        for (auto& statement : ast->statements)
        {
            evalStatement(statement, scope);
        }

        return makeModule(scope);
    }


    TSmartPtrType<Module> evalModule(const std::shared_ptr<parser::ModuleNode>& ast,const TSmartPtrType<ProgramScope>& program)
    {
        const auto scope = makeModuleScope(program);
        return evalModule(ast, scope);
    }

    TSmartPtrType<Prototype> evalClass(const std::shared_ptr<parser::PrototypeNode>& ast,
                                       const TSmartPtrType<ScopeLike>& scope)
    {
        auto prototype = makePrototype(scope, ast);
        scope->Create(ast->id, prototype);
        return prototype;
    }

    TSmartPtrType<DynamicObject> createDynamicFromPrototype(const std::shared_ptr<parser::PrototypeNode>& ast,
                                                            const TSmartPtrType<ScopeLike>& scope)
    {
        const auto dynamicObjScope = makeScope(scope);
        auto dynamicObj = makeDynamic(dynamicObjScope.CastStatic<ScopeLike>());

        for (auto& statement : ast->scope->statements)
        {
            evalStatement(statement, dynamicObjScope.CastStatic<ScopeLike>());
        }


        return dynamicObj;
    }

    TSmartPtrType<Object> eval(const std::shared_ptr<parser::Node>& ast)
    {
        switch (ast->type)
        {
        case parser::NT_Module:
            return evalModule(std::dynamic_pointer_cast<parser::ModuleNode>(ast),makeProgramScope());
        case parser::NT_Function:
            {
                return evalFunction(std::dynamic_pointer_cast<parser::FunctionNode>(ast), makeScope());
            }
        case parser::NT_Statement:
            {
                const auto scope = makeScope();
                return evalStatement(ast, scope.CastStatic<ScopeLike>());
            }
        default:
            {
                const auto scope = makeScope();
                return evalExpression(ast, scope.CastStatic<ScopeLike>());
            }
        }
    }
}
