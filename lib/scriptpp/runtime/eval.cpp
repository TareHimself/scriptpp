#include "scriptpp/runtime/eval.hpp"

#include <stdexcept>
#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/Boolean.hpp"
#include "scriptpp/runtime/Error.hpp"
#include "scriptpp/runtime/List.hpp"
#include "scriptpp/runtime/Null.hpp"
#include "scriptpp/runtime/Number.hpp"
#include "scriptpp/runtime/Prototype.hpp"
#include "scriptpp/runtime/String.hpp"

namespace spp::runtime
{

    std::shared_ptr<Object> evalBinaryOperation(const std::shared_ptr<frontend::BinaryOpNode>& ast,
                                              const std::shared_ptr<ScopeLike>& scope)
    {
        auto left = resolveReference(evalExpression(ast->left,scope));
        auto right = resolveReference(evalExpression(ast->right,scope));
        switch (ast->op)
        {
        case frontend::BinaryOpNode::BO_Divide:
            return left->Divide(right, scope);
        case frontend::BinaryOpNode::BO_Multiply:
            return left->Multiply(right, scope);
        case frontend::BinaryOpNode::BO_Add:
            return left->Add(right, scope);
        case frontend::BinaryOpNode::BO_Subtract:
            return left->Subtract(right, scope);
        case frontend::BinaryOpNode::BO_Mod:
            return left->Mod(right, scope);
        case frontend::BinaryOpNode::BO_And:
            return makeBoolean(left->ToBoolean(scope) && right->ToBoolean(scope));
        case frontend::BinaryOpNode::BO_Or:
            return makeBoolean(left->ToBoolean(scope) || right->ToBoolean(scope));
        case frontend::BinaryOpNode::BO_Not:
            throw std::runtime_error("This needs work");
        case frontend::BinaryOpNode::BO_Equal:
            return makeBoolean(left->Equal(right, scope));
        case frontend::BinaryOpNode::BO_NotEqual:
            return makeBoolean(left->NotEqual(right, scope));
        case frontend::BinaryOpNode::BO_Less:
            return makeBoolean(left->Less(right, scope));
        case frontend::BinaryOpNode::BO_LessEqual:
            return makeBoolean(left->LessEqual(right, scope));
        case frontend::BinaryOpNode::BO_Greater:
            return makeBoolean(left->Greater(right, scope));
        case frontend::BinaryOpNode::BO_GreaterEqual:
            return makeBoolean(left->GreaterEqual(right, scope));
        }

        return makeNull();
    }

    std::shared_ptr<Object> runScope(const std::shared_ptr<frontend::ScopeNode>& ast, const std::shared_ptr<ScopeLike>& scope)
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

    std::shared_ptr<Object> evalWhen(const std::shared_ptr<frontend::WhenNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        for (auto& [expression, statement] : ast->branches)
        {
            if (const auto e = evalExpression(expression, scope); e->ToBoolean(scope))
            {
                return evalStatement(statement, scope);
            }
        }

        return makeNull();
    }

    std::shared_ptr<Object> evalExpression(const std::shared_ptr<frontend::Node>& ast,
                                         const std::shared_ptr<ScopeLike>& scope)
    {
        switch (ast->type)
        {
        case frontend::NT_Variable:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::VariableNode>(ast))
                {
                    auto found = scope->Find(r->value);
                    if(!found) // every call to find should return a reference unless it was not found
                    {
                        throw makeError(scope,"\"" + r->value + "\" does not exist.",ast->debugInfo);
                    }
                    return found;
                }
                throw makeError(scope,"Expected variable",ast->debugInfo);
            }
        case frontend::NT_StringLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::LiteralNode>(ast))
                {
                    return makeString(r->value);
                }
                throw makeError(scope,"Expected string literal",ast->debugInfo);
            }
        case frontend::NT_NumericLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::LiteralNode>(ast))
                {
                    return makeNumber(r->value);
                }
                throw makeError(scope,"Expected numeric literal",ast->debugInfo);
            }
        case frontend::NT_BooleanLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::LiteralNode>(ast))
                {
                    return makeBoolean(r->value);
                }
                throw makeError(scope,"Expected boolean literal",ast->debugInfo);
            }
        case frontend::NT_ListLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::ListLiteralNode>(ast))
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
        case frontend::NT_NullLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::LiteralNode>(ast))
                {
                    return makeNull();
                }
                throw makeError(scope,"Expected null literal",ast->debugInfo);
            }
        case frontend::NT_BinaryOp:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::BinaryOpNode>(ast))
                {
                    return evalBinaryOperation(r, scope);
                }
                throw makeError(scope,"Expected binary operation",ast->debugInfo);
            }
        case frontend::NT_When:
            if (const auto r = std::dynamic_pointer_cast<frontend::WhenNode>(ast))
            {
                return evalWhen(r, scope);
            }
        case frontend::NT_Assign:
            if (const auto r = std::dynamic_pointer_cast<frontend::AssignNode>(ast))
            {
                return evalAssign(r, scope);
            }
        case frontend::NT_Function:
            if (const auto r = std::dynamic_pointer_cast<frontend::FunctionNode>(ast))
            {
                return evalFunction(r, scope);
            }
        case frontend::NT_Call:
            if (const auto r = std::dynamic_pointer_cast<frontend::CallNode>(ast))
            {
                return evalCall(r, scope);
            }
        case frontend::NT_Access:
            if (const auto r = std::dynamic_pointer_cast<frontend::AccessNode>(ast))
            {
                return evalAccess(r, scope);
            }
        case frontend::NT_Access2:
            if (const auto r = std::dynamic_pointer_cast<frontend::AccessNode2>(ast))
            {
                return evalAccess2(r, scope);
            }
        default:
            throw makeError(scope,"Unknown expression",ast->debugInfo);
        }
    }

    std::shared_ptr<Function> evalFunction(const std::shared_ptr<frontend::FunctionNode>& ast,
                                         const std::shared_ptr<ScopeLike>& scope)
    {
        return makeRuntimeFunction(scope, ast);
    }

    std::pair<std::shared_ptr<Function>, std::shared_ptr<ScopeLike>> resolveCallable(
        const std::shared_ptr<Object>& target,const std::shared_ptr<ScopeLike>& scope)
    {
        if (target->GetType() == OT_Function)
        {
            if (auto asCall = cast<Function>(target))
            {
                return {asCall,scope};
            }
        }

        if (target->GetType() == OT_Dynamic)
        {
            if (const auto asDynamic = cast<DynamicObject>(target))
            {
                if (asDynamic->Has(ReservedDynamicFunctions::CALL))
                {
                    if (auto asCall = cast<Function>(resolveReference(asDynamic->Get(ReservedDynamicFunctions::CALL))))
                    {
                        return {asCall,asDynamic};
                    }
                }
            }
        }

        return {};
    }

    std::shared_ptr<Object> callFunction(const std::shared_ptr<frontend::CallNode>& ast, const std::shared_ptr<Function>& fn,
                                         const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<CallScope>& callScope)
    {
        std::vector<std::shared_ptr<Object>> args;
        for (auto& arg : ast->args)
        {
            args.push_back(evalExpression(arg, scope));
        }

        return fn->Call(callScope,args);
    }
    

    std::shared_ptr<Object> evalCall(const std::shared_ptr<frontend::CallNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        
        if (const auto obj = evalExpression(ast->left, scope))
        {
            const auto target = resolveReference(obj);

            if(!target->IsCallable())
            {
                throw makeError(scope,target->ToString(scope) + " is not callable",ast->debugInfo);
            }

            if(auto [asCall,callScope] = resolveCallable(target,scope); asCall)
            {
                return callFunction(ast, asCall, scope,makeCallScope(ast->debugInfo,asCall,callScope));
            }
        }

        throw makeError(scope,"Call Failed",ast->debugInfo);
    }

    std::shared_ptr<Object> evalFor(const std::shared_ptr<frontend::ForNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        std::shared_ptr<Object> result = makeNull();
        evalStatement(ast->init, scope);
        while (resolveReference(evalStatement(ast->condition, scope))->ToBoolean(scope))
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

    std::shared_ptr<Object> evalWhile(const std::shared_ptr<frontend::WhileNode>& ast,
                                    const std::shared_ptr<ScopeLike>& scope)
    {
        std::shared_ptr<Object> result = makeNull();
        while (resolveReference(evalStatement(ast->condition, scope))->ToBoolean(scope))
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

    std::shared_ptr<Object> evalStatement(const std::shared_ptr<frontend::Node>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        switch (ast->type)
        {
        case frontend::NT_CreateAndAssign:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::CreateAndAssignNode>(ast))
                {
                    auto result = evalExpression(a->value, scope);
                    scope->Assign(a->name, resolveReference(result));
                    return result;
                }
            }
            break;
        case frontend::NT_Function:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::FunctionNode>(ast))
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
        case frontend::NT_Call:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::CallNode>(ast))
                {
                    return evalCall(a, scope);
                }
            }
            break;
        case frontend::NT_Scope:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::ScopeNode>(ast))
                {
                    return runScope(a, scope);
                }
            }
            break;
        case frontend::NT_Return:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::ReturnNode>(ast))
                {
                    if (scope->HasScopeType(ST_Function))
                    {
                        return makeReturnValue(evalExpression(a->expression, scope));
                    }
                    return evalExpression(a->expression, scope);
                }
            }
            break;
        case frontend::NT_Throw:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::ThrowNode>(ast))
                {
                    throw makeError(scope,evalExpression(a->expression,scope),ast->debugInfo);
                }
            }
            break;
        case frontend::NT_TryCatch:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::TryCatchNode>(ast))
                {
                    return evalTryCatch(a,scope);
                }
            }
            break;
        case frontend::NT_When:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::WhenNode>(ast))
                {
                    return evalWhen(a, scope);
                }
            }
            break;
        case frontend::NT_For:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::ForNode>(ast))
                {
                    return evalFor(a, scope);
                }
            }
            break;
        case frontend::NT_While:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::WhileNode>(ast))
                {
                    return evalWhile(a, scope);
                }
            }
            break;
        case frontend::NT_Break:
            return makeFlowControl(FlowControl::Break);
        case frontend::NT_Continue:
            return makeFlowControl(FlowControl::Continue);
        case frontend::NT_Class:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::PrototypeNode>(ast))
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

    std::shared_ptr<Object> evalAccess(const std::shared_ptr<frontend::AccessNode>& ast,
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

        throw makeError(scope,target->ToString(scope) + " is not a dynamic object",ast->debugInfo);
    }

    std::shared_ptr<Object> evalAccess2(const std::shared_ptr<frontend::AccessNode2>& ast,
                                      const std::shared_ptr<ScopeLike>& scope)
    {
        const auto target = evalExpression(ast->left, scope);
        if (const auto rTarget = cast<DynamicObject>(resolveReference(target)))
        {
            const auto within = evalExpression(ast->within, scope);
            const auto rWithin = resolveReference(within);

            if(auto getResult = rTarget->Get(rWithin))
            {
                return getResult;
            }
            
            throw makeError(scope, rWithin->ToString(scope) + " cannot be used to index " + rTarget->ToString(scope),ast->debugInfo);
        }

        throw makeError(scope,target->ToString(scope) + " is not a dynamic object",ast->debugInfo);
    }

    std::shared_ptr<Object> evalAssign(const std::shared_ptr<frontend::AssignNode>& ast,
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

    std::shared_ptr<Object> evalTryCatch(const std::shared_ptr<frontend::TryCatchNode>& ast,
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

    std::shared_ptr<Module> evalModule(const std::shared_ptr<frontend::ModuleNode>& ast,
                                     const std::shared_ptr<Program>& program)
    {
        auto mod = makeModule(program);
        
        for (auto& statement : ast->statements)
        {
            evalStatement(statement, mod);
        }

        return mod;
    }
    
    std::shared_ptr<Prototype> evalClass(const std::shared_ptr<frontend::PrototypeNode>& ast,
                                       const std::shared_ptr<ScopeLike>& scope)
    {
        auto prototype = makePrototype(scope, ast);
        scope->Create(ast->id, prototype);
        return prototype;
    }

    std::shared_ptr<DynamicObject> createDynamicFromPrototype(const std::shared_ptr<frontend::PrototypeNode>& ast,
                                                            const std::shared_ptr<ScopeLike>& scope)
    {
        auto dynamicObj = makeDynamic(scope);

        for (auto& statement : ast->scope->statements)
        {
            evalStatement(statement, dynamicObj);
        }
        
        return dynamicObj;
    }

    std::shared_ptr<Object> eval(const std::shared_ptr<frontend::Node>& ast)
    {
        switch (ast->type)
        {
        case frontend::NT_Module:
            {
                const auto program = makeProgram();
                return evalModule(std::dynamic_pointer_cast<frontend::ModuleNode>(ast),program);
                
            }
        case frontend::NT_Function:
            {
                return evalFunction(std::dynamic_pointer_cast<frontend::FunctionNode>(ast), makeScope());
            }
        case frontend::NT_Statement:
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
