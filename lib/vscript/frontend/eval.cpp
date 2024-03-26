#include "vscript/frontend/eval.hpp"

#include <stdexcept>

#include "vscript/frontend/Boolean.hpp"
#include "vscript/frontend/List.hpp"
#include "vscript/frontend/Null.hpp"
#include "vscript/frontend/Number.hpp"
#include "vscript/frontend/Prototype.hpp"
#include "vscript/frontend/String.hpp"

namespace vs::frontend
{

    TSmartPtrType<Object> evalBinaryOperation(const std::shared_ptr<backend::BinaryOpNode>& ast,
                                              const TSmartPtrType<ScopeLike>& scope)
    {
        switch (ast->op)
        {
        case backend::BinaryOpNode::BO_Divide:
            return evalExpression(ast->left, scope)->Divide(evalExpression(ast->right, scope));
        case backend::BinaryOpNode::BO_Multiply:
            return evalExpression(ast->left, scope)->Multiply(evalExpression(ast->right, scope));
        case backend::BinaryOpNode::BO_Add:
            return evalExpression(ast->left, scope)->Add(evalExpression(ast->right, scope));
        case backend::BinaryOpNode::BO_Subtract:
            return evalExpression(ast->left, scope)->Subtract(evalExpression(ast->right, scope));
        case backend::BinaryOpNode::BO_Mod:
            return evalExpression(ast->left, scope)->Subtract(evalExpression(ast->right, scope));
        case backend::BinaryOpNode::BO_And:
            return makeBoolean(
                evalExpression(ast->left, scope)->ToBoolean() && evalExpression(ast->right, scope)->ToBoolean());
        case backend::BinaryOpNode::BO_Or:
            return makeBoolean(
                evalExpression(ast->left, scope)->ToBoolean() || evalExpression(ast->right, scope)->ToBoolean());
        case backend::BinaryOpNode::BO_Not:
            throw std::runtime_error("This need work");
        case backend::BinaryOpNode::BO_Equal:
            return makeBoolean(evalExpression(ast->left, scope)->Equal(evalExpression(ast->right, scope)));
        case backend::BinaryOpNode::BO_NotEqual:
            return makeBoolean(evalExpression(ast->left, scope)->NotEqual(evalExpression(ast->right, scope)));
        case backend::BinaryOpNode::BO_Less:
            return makeBoolean(evalExpression(ast->left, scope)->Less(evalExpression(ast->right, scope)));
        case backend::BinaryOpNode::BO_LessEqual:
            return makeBoolean(evalExpression(ast->left, scope)->LessEqual(evalExpression(ast->right, scope)));
        case backend::BinaryOpNode::BO_Greater:
            return makeBoolean(evalExpression(ast->left, scope)->Greater(evalExpression(ast->right, scope)));
        case backend::BinaryOpNode::BO_GreaterEqual:
            return makeBoolean(evalExpression(ast->left, scope)->GreaterEqual(evalExpression(ast->right, scope)));
        }

        return makeNull();
    }

    TSmartPtrType<Object> runScope(const std::shared_ptr<backend::ScopeNode>& ast, const TSmartPtrType<ScopeLike>& scope)
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

    TSmartPtrType<Object> evalWhen(const std::shared_ptr<backend::WhenNode>& ast, const TSmartPtrType<ScopeLike>& scope)
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

    TSmartPtrType<Object> evalExpression(const std::shared_ptr<backend::Node>& ast,
                                         const TSmartPtrType<ScopeLike>& scope)
    {
        switch (ast->type)
        {
        case backend::NT_Variable:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::VariableNode>(ast))
                {
                    return scope->Find(r->value);
                }
                throw std::runtime_error("Expected variable");
            }
        case backend::NT_StringLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::LiteralNode>(ast))
                {
                    return makeString(r->value);
                }
                throw std::runtime_error("Expected string literal");
            }
        case backend::NT_NumericLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::LiteralNode>(ast))
                {
                    return makeNumber(r->value);
                }
                throw std::runtime_error("Expected numeric literal");
            }
        case backend::NT_BooleanLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::LiteralNode>(ast))
                {
                    return makeBoolean(r->value);
                }
                throw std::runtime_error("Expected boolean literal");
            }
        case backend::NT_NullLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::LiteralNode>(ast))
                {
                    return makeNull();
                }
                throw std::runtime_error("Expected null literal");
            }
        case backend::NT_BinaryOp:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::BinaryOpNode>(ast))
                {
                    return evalBinaryOperation(r, scope);
                }
                throw std::runtime_error("Expected binary operation");
            }
        case backend::NT_When:
            if (const auto r = std::dynamic_pointer_cast<backend::WhenNode>(ast))
            {
                return evalWhen(r, scope);
            }
        case backend::NT_Assign:
            if (const auto r = std::dynamic_pointer_cast<backend::AssignNode>(ast))
            {
                return evalAssign(r, scope);
            }
        case backend::NT_Function:
            if (const auto r = std::dynamic_pointer_cast<backend::FunctionNode>(ast))
            {
                return evalFunction(r, scope);
            }
        case backend::NT_Call:
            if (const auto r = std::dynamic_pointer_cast<backend::CallNode>(ast))
            {
                return evalCall(r, scope);
            }
        case backend::NT_Access:
            if (const auto r = std::dynamic_pointer_cast<backend::AccessNode>(ast))
            {
                return evalAccess(r, scope);
            }
        case backend::NT_Access2:
            if (const auto r = std::dynamic_pointer_cast<backend::AccessNode2>(ast))
            {
                return evalAccess2(r, scope);
            }
        default:
            throw std::runtime_error("Unknown expression");
        }
    }

    TSmartPtrType<Function> evalFunction(const std::shared_ptr<backend::FunctionNode>& ast,
                                         const TSmartPtrType<ScopeLike>& scope)
    {
        return makeRuntimeFunction(scope, ast->name, ast->args, ast->body);
    }

    TSmartPtrType<Object> callFunction(const std::shared_ptr<backend::CallNode>& ast, const TSmartPtrType<Function>& fn,
                                       const TSmartPtrType<ScopeLike>& scope)
    {
        std::vector<TSmartPtrType<Object>> args;
        for (auto& arg : ast->args)
        {
            args.push_back(evalExpression(arg, scope));
        }

        return fn->Call(args);
    }
    

    TSmartPtrType<Object> evalCall(const std::shared_ptr<backend::CallNode>& ast, const TSmartPtrType<ScopeLike>& scope)
    {
        
        if (const auto obj = evalExpression(ast->left, scope); obj.IsValid())
        {
            auto target = resolveReference(obj);
            
            if (target->GetType() == OT_Function)
            {
                if (const auto asCall = target.Cast<Function>(); asCall.IsValid())
                {
                    return callFunction(ast, asCall, scope);
                }
            }

            if (target->GetType() == OT_Dynamic)
            {
                if (const auto asDynamic = target.Cast<DynamicObject>(); asDynamic.IsValid())
                {
                    if (asDynamic->Has(ReservedDynamicFunctions::CALL))
                    {
                        if (const auto asCall = resolveReference(asDynamic->Get(ReservedDynamicFunctions::CALL)).Cast<Function>(); asCall.IsValid())
                        {
                            return callFunction(ast, asCall, scope);
                        }
                    }
                }
            }

            throw std::runtime_error(target->ToString() + " is not callable");
        }

        throw std::runtime_error("Unknown object during call");
    }

    TSmartPtrType<Object> evalFor(const std::shared_ptr<backend::ForNode>& ast, const TSmartPtrType<ScopeLike>& scope)
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

    TSmartPtrType<Object> evalWhile(const std::shared_ptr<backend::WhileNode>& ast,
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

    TSmartPtrType<Object> evalStatement(const std::shared_ptr<backend::Node>& ast, const TSmartPtrType<ScopeLike>& scope)
    {
        switch (ast->type)
        {
        case backend::NT_CreateAndAssign:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::CreateAndAssignNode>(ast))
                {
                    auto result = evalExpression(a->value, scope);
                    scope->Assign(a->name, resolveReference(result));
                    return result;
                }
            }
            break;
        case backend::NT_Function:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::FunctionNode>(ast))
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
        case backend::NT_Call:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::CallNode>(ast))
                {
                    return evalCall(a, scope);
                }
            }
            break;
        case backend::NT_Scope:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::ScopeNode>(ast))
                {
                    return runScope(a, scope);
                }
            }
            break;
        case backend::NT_Return:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::ReturnNode>(ast))
                {
                    if (scope->HasScopeType(ST_Function))
                    {
                        return makeReturnValue(evalExpression(a->expression, scope));
                    }
                    return evalExpression(a->expression, scope);
                }
            }
            break;
        case backend::NT_When:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::WhenNode>(ast))
                {
                    return evalWhen(a, scope);
                }
            }
            break;
        case backend::NT_For:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::ForNode>(ast))
                {
                    return evalFor(a, scope);
                }
            }
            break;
        case backend::NT_While:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::WhileNode>(ast))
                {
                    return evalWhile(a, scope);
                }
            }
            break;
        case backend::NT_Break:
            return makeFlowControl(FlowControl::Break);
        case backend::NT_Continue:
            return makeFlowControl(FlowControl::Continue);
        case backend::NT_Class:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::PrototypeNode>(ast))
                {
                    return evalClass(a, scope);
                }
            }
            break;
        default:
            return evalExpression(ast, scope);
        }
        
        return makeNull();
    }

    TSmartPtrType<Object> evalAccess(const std::shared_ptr<backend::AccessNode>& ast,
                                     const TSmartPtrType<ScopeLike>& scope)
    {
        auto target = evalExpression(ast->left, scope);
        if (const auto rTarget = resolveReference(target); rTarget->GetType() == OT_Dynamic)
        {
            if (const auto asDynamic = rTarget.Cast<DynamicObject>(); asDynamic.IsValid())
            {
                return evalExpression(ast->right, asDynamic);
            }
        }

        throw std::runtime_error(target->ToString() + " is not a dynamic object");
    }

    TSmartPtrType<Object> evalAccess2(const std::shared_ptr<backend::AccessNode2>& ast,
                                      const TSmartPtrType<ScopeLike>& scope)
    {
        auto target = evalExpression(ast->left, scope);
        if (const auto rTarget = resolveReference(target); rTarget->GetType() == OT_Dynamic)
        {
            const auto within = evalExpression(ast->within, scope);
            const auto rWithin = resolveReference(within);

            if (auto dyn = rTarget.Cast<DynamicObject>(); dyn.IsValid())
            {
                return dyn->Get(within);
            }
        }

        throw std::runtime_error(target->ToString() + " is not a dynamic object");
    }

    TSmartPtrType<Object> evalAssign(const std::shared_ptr<backend::AssignNode>& ast,
                                     const TSmartPtrType<ScopeLike>& scope)
    {
        if(auto left = evalExpression(ast->left,scope); left->GetType() == OT_Reference)
        {
            const auto right = evalExpression(ast->value,scope);
            const auto trueRight = resolveReference(right);
            left.Cast<Reference>()->Set(trueRight);
            return right;
        }
        
        // if (ast->left->type == parser::NT_Access)
        // {
        //     if (const auto accessNode = std::dynamic_pointer_cast<parser::AccessNode>(ast->left))
        //     {
        //         if (accessNode->right->type == parser::NT_Variable)
        //         {
        //             if (auto accessed = evalExpression(accessNode->left, scope).Cast<DynamicObject>(); accessed.
        //                 IsValid())
        //             {
        //                 const auto var = std::dynamic_pointer_cast<parser::VariableNode>(accessNode->right);
        //                 auto val = evalExpression(ast->value, scope);
        //                 accessed->Assign(var->value, val);
        //                 return val;
        //             }
        //         }
        //     }
        // }
        // else if (ast->left->type == parser::NT_Access2)
        // {
        //     if (const auto accessNode = std::dynamic_pointer_cast<parser::AccessNode2>(ast->left))
        //     {
        //         if (auto target = evalExpression(accessNode->left, scope); target->GetType() == OT_Dynamic)
        //         {
        //             if (auto dyn = target.Cast<DynamicObject>(); dyn.IsValid())
        //             {
        //                 auto val = evalExpression(ast->value, scope);
        //                 dyn->Set(evalExpression(accessNode->within, scope), val);
        //                 return val;
        //             }
        //         }
        //     }
        // }
        // else if (ast->left->type == parser::NT_Variable)
        // {
        //     const auto var = std::dynamic_pointer_cast<parser::VariableNode>(ast->left);
        //     auto val = evalExpression(ast->value, scope);
        //     scope->Assign(var->value, val);
        //     return val;
        // }

        throw std::runtime_error("Assign failed");
    }

    TSmartPtrType<Module> evalModule(const std::shared_ptr<backend::ModuleNode>& ast,
                                     const TSmartPtrType<Program>& program)
    {
        auto mod = makeModule(program);
        
        for (auto& statement : ast->statements)
        {
            evalStatement(statement, mod);
        }

        return mod;
    }
    
    TSmartPtrType<Prototype> evalClass(const std::shared_ptr<backend::PrototypeNode>& ast,
                                       const TSmartPtrType<ScopeLike>& scope)
    {
        auto prototype = makePrototype(scope, ast);
        scope->Create(ast->id, prototype);
        return prototype;
    }

    TSmartPtrType<DynamicObject> createDynamicFromPrototype(const std::shared_ptr<backend::PrototypeNode>& ast,
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

    TSmartPtrType<Object> eval(const std::shared_ptr<backend::Node>& ast)
    {
        switch (ast->type)
        {
        case backend::NT_Module:
            {
                const auto prog = makeProgram();
                return evalModule(std::dynamic_pointer_cast<backend::ModuleNode>(ast),prog);
            }
        case backend::NT_Function:
            {
                return evalFunction(std::dynamic_pointer_cast<backend::FunctionNode>(ast), makeScope());
            }
        case backend::NT_Statement:
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
