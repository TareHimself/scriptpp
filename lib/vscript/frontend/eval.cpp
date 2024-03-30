#include "vscript/frontend/eval.hpp"

#include <stdexcept>

#include "vscript/utils.hpp"
#include "vscript/frontend/Boolean.hpp"
#include "vscript/frontend/Error.hpp"
#include "vscript/frontend/List.hpp"
#include "vscript/frontend/Null.hpp"
#include "vscript/frontend/Number.hpp"
#include "vscript/frontend/Prototype.hpp"
#include "vscript/frontend/String.hpp"

namespace vs::frontend
{

    std::shared_ptr<Object> evalBinaryOperation(const std::shared_ptr<backend::BinaryOpNode>& ast,
                                              const std::shared_ptr<ScopeLike>& scope)
    {
        auto left = resolveReference(evalExpression(ast->left,scope));
        auto right = resolveReference(evalExpression(ast->right,scope));
        switch (ast->op)
        {
        case backend::BinaryOpNode::BO_Divide:
            return left->Divide(right);
        case backend::BinaryOpNode::BO_Multiply:
            return left->Multiply(right);
        case backend::BinaryOpNode::BO_Add:
            return left->Add(right);
        case backend::BinaryOpNode::BO_Subtract:
            return left->Subtract(right);
        case backend::BinaryOpNode::BO_Mod:
            return left->Mod(right);
        case backend::BinaryOpNode::BO_And:
            return makeBoolean(left->ToBoolean() && right->ToBoolean());
        case backend::BinaryOpNode::BO_Or:
            return makeBoolean(left->ToBoolean() || right->ToBoolean());
        case backend::BinaryOpNode::BO_Not:
            throw std::runtime_error("This needs work");
        case backend::BinaryOpNode::BO_Equal:
            return makeBoolean(left->Equal(right));
        case backend::BinaryOpNode::BO_NotEqual:
            return makeBoolean(left->NotEqual(right));
        case backend::BinaryOpNode::BO_Less:
            return makeBoolean(left->Less(right));
        case backend::BinaryOpNode::BO_LessEqual:
            return makeBoolean(left->LessEqual(right));
        case backend::BinaryOpNode::BO_Greater:
            return makeBoolean(left->Greater(right));
        case backend::BinaryOpNode::BO_GreaterEqual:
            return makeBoolean(left->GreaterEqual(right));
        }

        return makeNull();
    }

    std::shared_ptr<Object> runScope(const std::shared_ptr<backend::ScopeNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        std::shared_ptr<Object> lastResult = makeNull();

        for (const auto& statement : ast->statements)
        {
            if (auto evalResult = evalStatement(statement, scope))
            {
                if (evalResult->GetType() == OT_ReturnValue)
                {
                    if (scope->GetScopeType() == ST_Function)
                    {
                        return cast<ReturnValue>(evalResult)->GetValue();
                    }

                    return evalResult;
                }

                lastResult = evalResult;
            }
        }

        return lastResult;
    }

    std::shared_ptr<Object> evalWhen(const std::shared_ptr<backend::WhenNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        for (auto& branch : ast->branches)
        {
            auto e = evalExpression(branch.expression, scope);
            if (e->ToBoolean())
            {
                return evalStatement(branch.statement, scope);
            }
        }

        return makeNull();
    }

    std::shared_ptr<Object> evalExpression(const std::shared_ptr<backend::Node>& ast,
                                         const std::shared_ptr<ScopeLike>& scope)
    {
        switch (ast->type)
        {
        case backend::NT_Variable:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::VariableNode>(ast))
                {
                    return scope->Find(r->value);
                }
                throw makeError(scope,"Expected variable",ast->debugInfo);
            }
        case backend::NT_StringLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::LiteralNode>(ast))
                {
                    return makeString(r->value);
                }
                throw makeError(scope,"Expected string literal",ast->debugInfo);
            }
        case backend::NT_NumericLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::LiteralNode>(ast))
                {
                    return makeNumber(r->value);
                }
                throw makeError(scope,"Expected numeric literal",ast->debugInfo);
            }
        case backend::NT_BooleanLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::LiteralNode>(ast))
                {
                    return makeBoolean(r->value);
                }
                throw makeError(scope,"Expected boolean literal",ast->debugInfo);
            }
        case backend::NT_ListLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::ListLiteralNode>(ast))
                {
                    std::vector<std::shared_ptr<Object>> items;
                    for(auto &it : r->values)
                    {
                        auto evalResult = evalExpression(it,scope);
                        items.push_back(resolveReference(evalResult));
                    }
                    return makeList(items);
                }
                throw makeError(scope,"Expected list literal",ast->debugInfo);
            }
        case backend::NT_NullLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::LiteralNode>(ast))
                {
                    return makeNull();
                }
                throw makeError(scope,"Expected null literal",ast->debugInfo);
            }
        case backend::NT_BinaryOp:
            {
                if (const auto r = std::dynamic_pointer_cast<backend::BinaryOpNode>(ast))
                {
                    return evalBinaryOperation(r, scope);
                }
                throw makeError(scope,"Expected binary operation",ast->debugInfo);
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
            throw makeError(scope,"Unknown expression",ast->debugInfo);
        }
    }

    std::shared_ptr<Function> evalFunction(const std::shared_ptr<backend::FunctionNode>& ast,
                                         const std::shared_ptr<ScopeLike>& scope)
    {
        return makeRuntimeFunction(scope, ast);
    }

    std::shared_ptr<Object> callFunction(const std::shared_ptr<backend::CallNode>& ast, const std::shared_ptr<Function>& fn,
                                       const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<CallScope>& callScope)
    {
        std::vector<std::shared_ptr<Object>> args;
        for (auto& arg : ast->args)
        {
            args.push_back(evalExpression(arg, scope));
        }

        return fn->Call(callScope,args);
    }
    

    std::shared_ptr<Object> evalCall(const std::shared_ptr<backend::CallNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        
        if (const auto obj = evalExpression(ast->left, scope))
        {
            const auto target = resolveReference(obj);
            
            if (target->GetType() == OT_Function)
            {
                if (const auto asCall = cast<Function>(target))
                {
                    return callFunction(ast, asCall, scope,makeCallScope(ast->debugInfo,asCall,scope));
                }
            }

            if (target->GetType() == OT_Dynamic)
            {
                if (const auto asDynamic = cast<DynamicObject>(target))
                {
                    if (asDynamic->Has(ReservedDynamicFunctions::CALL))
                    {
                        if (const auto asCall = cast<Function>(resolveReference(asDynamic->Get(ReservedDynamicFunctions::CALL))))
                        {
                            return callFunction(ast, asCall, scope,makeCallScope(ast->debugInfo,asCall,asDynamic));
                        }
                    }
                }
            }

            throw makeError(scope,target->ToString() + " is not callable",ast->debugInfo);
        }

        throw makeError(scope,"Unknown object during call",ast->debugInfo);
    }

    std::shared_ptr<Object> evalFor(const std::shared_ptr<backend::ForNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        std::shared_ptr<Object> result = makeNull();
        evalStatement(ast->init, scope);
        while (resolveReference(evalStatement(ast->condition, scope))->ToBoolean())
        {
            if (auto temp = resolveReference(runScope(ast->body, scope)))
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
                    if (const auto flow = cast<FlowControl>(temp))
                    {
                        if (flow->GetValue() == FlowControl::Break)
                        {
                            break;
                        }
                        else if (flow->GetValue() == FlowControl::Continue)
                        {
                            evalStatement(ast->update, scope);
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

    std::shared_ptr<Object> evalWhile(const std::shared_ptr<backend::WhileNode>& ast,
                                    const std::shared_ptr<ScopeLike>& scope)
    {
        std::shared_ptr<Object> result = makeNull();
        while (resolveReference(evalStatement(ast->condition, scope))->ToBoolean())
        {
            if (auto temp = runScope(ast->body, scope))
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
                    if (const auto flow = cast<FlowControl>(temp))
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

    std::shared_ptr<Object> evalStatement(const std::shared_ptr<backend::Node>& ast, const std::shared_ptr<ScopeLike>& scope)
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
        case backend::NT_Throw:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::ThrowNode>(ast))
                {
                    throw makeError(scope,evalExpression(a->expression,scope),ast->debugInfo);
                }
            }
            break;
        case backend::NT_TryCatch:
            {
                if (const auto a = std::dynamic_pointer_cast<backend::TryCatchNode>(ast))
                {
                    return evalTryCatch(a,scope);
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

    std::shared_ptr<Object> evalAccess(const std::shared_ptr<backend::AccessNode>& ast,
                                     const std::shared_ptr<ScopeLike>& scope)
    {
        auto target = evalExpression(ast->left, scope);
        if (const auto rTarget = cast<DynamicObject>(resolveReference(target)))
        {
            return evalExpression(ast->right, rTarget);
            // if (const auto asDynamic = rTarget.Cast<DynamicObject>(); asDynamic.IsValid())
            // {
            //     
            // }
        }

        throw makeError(scope,target->ToString() + " is not a dynamic object",ast->debugInfo);
    }

    std::shared_ptr<Object> evalAccess2(const std::shared_ptr<backend::AccessNode2>& ast,
                                      const std::shared_ptr<ScopeLike>& scope)
    {
        auto target = evalExpression(ast->left, scope);
        if (const auto rTarget = cast<DynamicObject>(resolveReference(target)))
        {
            const auto within = evalExpression(ast->within, scope);
            const auto rWithin = resolveReference(within);

            return rTarget->Get(rWithin);
        }

        throw makeError(scope,target->ToString() + " is not a dynamic object",ast->debugInfo);
    }

    std::shared_ptr<Object> evalAssign(const std::shared_ptr<backend::AssignNode>& ast,
                                     const std::shared_ptr<ScopeLike>& scope)
    {
        if(auto left = evalExpression(ast->left,scope); left->GetType() == OT_Reference)
        {
            auto right = evalExpression(ast->value,scope);
            const auto trueRight = resolveReference(right);
            cast<Reference>(left)->Set(trueRight);
            return right;
        }
        
        throw makeError(scope,"Assign failed",ast->debugInfo);
    }

    std::shared_ptr<Object> evalTryCatch(const std::shared_ptr<backend::TryCatchNode>& ast,
        const std::shared_ptr<ScopeLike>& scope)
    {
        try
        {
            return runScope(ast->tryScope,scope);
        }
        catch (std::shared_ptr<Error> & e)
        {
            auto catchScope = makeScope(scope);
            if(!ast->catchArgumentName.empty())
            {
                catchScope->Create(ast->catchArgumentName,e);
            }
            return runScope(ast->catchScope,catchScope);
        }
    }

    std::shared_ptr<Module> evalModule(const std::shared_ptr<backend::ModuleNode>& ast,
                                     const std::shared_ptr<Program>& program)
    {
        auto mod = makeModule(program);
        
        for (auto& statement : ast->statements)
        {
            evalStatement(statement, mod);
        }

        return mod;
    }
    
    std::shared_ptr<Prototype> evalClass(const std::shared_ptr<backend::PrototypeNode>& ast,
                                       const std::shared_ptr<ScopeLike>& scope)
    {
        auto prototype = makePrototype(scope, ast);
        scope->Create(ast->id, prototype);
        return prototype;
    }

    std::shared_ptr<DynamicObject> createDynamicFromPrototype(const std::shared_ptr<backend::PrototypeNode>& ast,
                                                            const std::shared_ptr<ScopeLike>& scope)
    {
        const auto dynamicObjScope = makeScope(scope);
        auto dynamicObj = makeDynamic(dynamicObjScope);

        for (auto& statement : ast->scope->statements)
        {
            evalStatement(statement, dynamicObjScope);
        }


        return dynamicObj;
    }

    std::shared_ptr<Object> eval(const std::shared_ptr<backend::Node>& ast)
    {
        switch (ast->type)
        {
        case backend::NT_Module:
            {
                const auto program = makeProgram();
                return evalModule(std::dynamic_pointer_cast<backend::ModuleNode>(ast),program);
                
            }
        case backend::NT_Function:
            {
                return evalFunction(std::dynamic_pointer_cast<backend::FunctionNode>(ast), makeScope());
            }
        case backend::NT_Statement:
            {
                const auto scope = makeScope();
                return evalStatement(ast, scope);
            }
        default:
            {
                const auto scope = makeScope();
                return evalExpression(ast, scope);
            }
        }
    }
}
