#include "scriptpp/runtime/eval.hpp"

#include <stdexcept>
#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/Boolean.hpp"
#include "scriptpp/runtime/Exception.hpp"
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
        case frontend::EBinaryOp::Divide:
            return left->Divide(right, scope);
        case frontend::EBinaryOp::Multiply:
            return left->Multiply(right, scope);
        case frontend::EBinaryOp::Add:
            return left->Add(right, scope);
        case frontend::EBinaryOp::Subtract:
            return left->Subtract(right, scope);
        case frontend::EBinaryOp::Mod:
            return left->Mod(right, scope);
        case frontend::EBinaryOp::And:
            return makeBoolean(left->ToBoolean(scope) && right->ToBoolean(scope));
        case frontend::EBinaryOp::Or:
            return makeBoolean(left->ToBoolean(scope) || right->ToBoolean(scope));
        case frontend::EBinaryOp::Not:
            throw std::runtime_error("This needs work");
        case frontend::EBinaryOp::Equal:
            return makeBoolean(left->Equal(right, scope));
        case frontend::EBinaryOp::NotEqual:
            return makeBoolean(left->NotEqual(right, scope));
        case frontend::EBinaryOp::Less:
            return makeBoolean(left->Less(right, scope));
        case frontend::EBinaryOp::LessEqual:
            return makeBoolean(left->LessEqual(right, scope));
        case frontend::EBinaryOp::Greater:
            return makeBoolean(left->Greater(right, scope));
        case frontend::EBinaryOp::GreaterEqual:
            return makeBoolean(left->GreaterEqual(right, scope));
        }

        return makeNull();
    }

    std::shared_ptr<Object> runScope(const std::shared_ptr<frontend::ScopeNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        std::shared_ptr<Object> lastResult{};

        for (const auto& statement : ast->statements)
        {
            if (auto evalResult = evalStatement(statement, scope))
            {
                if (evalResult->GetType() == EObjectType::ReturnValue)
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

        return lastResult ? lastResult : makeNull();
    }

    std::shared_ptr<Object> evalScope(const std::shared_ptr<frontend::ScopeNode>& ast,
        const std::shared_ptr<ScopeLike>& outerScope)
    {
        std::shared_ptr<Object> lastResult{};
        auto scope = makeScope(outerScope);

        for (const auto& statement : ast->statements)
        {
            if (auto evalResult = evalStatement(statement, scope))
            {
                lastResult = evalResult;
            }
        }

        return lastResult ? lastResult : makeNull();
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
        case  frontend::ENodeType::NoOp:
            {
                return makeBoolean(true);
            }
        case frontend::ENodeType::Identifier:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::IdentifierNode>(ast))
                {
                    auto found = scope->Find(r->value);
                    if(!found) // every call to find should return a reference unless it was not found
                    {
                        throw makeException(scope,"\"" + r->value + "\" does not exist.",ast->debugInfo);
                    }
                    return found;
                }
                throw makeException(scope,"Expected variable",ast->debugInfo);
            }
        case frontend::ENodeType::StringLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::StringLiteralNode>(ast))
                {
                    return makeString(r->value);
                }
                throw makeException(scope,"Expected string literal",ast->debugInfo);
            }
        case frontend::ENodeType::NumericLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::NumericLiteralNode>(ast))
                {
                    return makeNumber(r->value);
                }
                throw makeException(scope,"Expected numeric literal",ast->debugInfo);
            }
        case frontend::ENodeType::BooleanLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::BooleanLiteralNode>(ast))
                {
                    return makeBoolean(r->value);
                }
                throw makeException(scope,"Expected boolean literal",ast->debugInfo);
            }
        case frontend::ENodeType::ListLiteral:
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
                throw makeException(scope,"Expected list literal",ast->debugInfo);
            }
        case frontend::ENodeType::NullLiteral:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::NullLiteralNode>(ast))
                {
                    return makeNull();
                }
                throw makeException(scope,"Expected null literal",ast->debugInfo);
            }
        case frontend::ENodeType::BinaryOp:
            {
                if (const auto r = std::dynamic_pointer_cast<frontend::BinaryOpNode>(ast))
                {
                    return evalBinaryOperation(r, scope);
                }
                throw makeException(scope,"Expected binary operation",ast->debugInfo);
            }
        case frontend::ENodeType::When:
            if (const auto r = std::dynamic_pointer_cast<frontend::WhenNode>(ast))
            {
                return evalWhen(r, scope);
            }
        case frontend::ENodeType::Assign:
            if (const auto r = std::dynamic_pointer_cast<frontend::AssignNode>(ast))
            {
                return evalAssign(r, scope);
            }
        case frontend::ENodeType::Function:
            if (const auto r = std::dynamic_pointer_cast<frontend::FunctionNode>(ast))
            {
                return evalFunction(r, scope);
            }
        case frontend::ENodeType::Call:
            if (const auto r = std::dynamic_pointer_cast<frontend::CallNode>(ast))
            {
                return evalCall(r, scope);
            }
        case frontend::ENodeType::Access:
            if (const auto r = std::dynamic_pointer_cast<frontend::AccessNode>(ast))
            {
                return evalAccess(r, scope);
            }
        case frontend::ENodeType::Index:
            if (const auto r = std::dynamic_pointer_cast<frontend::IndexNode>(ast))
            {
                return evalIndex(r, scope);
            }
        case frontend::ENodeType::Scope:
            if (const auto r = std::dynamic_pointer_cast<frontend::ScopeNode>(ast))
            {
                return evalScope(r, scope);
            }
        case frontend::ENodeType::CreateAndAssign:
            if (const auto r = std::dynamic_pointer_cast<frontend::CreateAndAssignNode>(ast))
            {
                return evalCreateAndAssign(r, scope);
            }
        default:
            throw makeException(scope,"Unknown expression",ast->debugInfo);
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
        if (target->GetType() == EObjectType::Function)
        {
            if (auto asCall = cast<Function>(target))
            {
                return {asCall,scope};
            }
        }

        if (target->GetType() == EObjectType::Dynamic)
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
                throw makeException(scope,target->ToString(scope) + " is not callable",ast->debugInfo);
            }

            if(auto [asCall,callScope] = resolveCallable(target,scope); asCall)
            {
                return callFunction(ast, asCall, scope,makeCallScope(ast->debugInfo,asCall,callScope));
            }
        }

        throw makeException(scope,"Call Failed",ast->debugInfo);
    }

    std::shared_ptr<Object> evalFor(const std::shared_ptr<frontend::ForNode>& ast, const std::shared_ptr<ScopeLike>& scope)
    {
        std::shared_ptr<Object> result = makeNull();
        evalStatement(ast->init, scope);
        while (resolveReference(evalStatement(ast->condition, scope))->ToBoolean(scope))
        {
            if (auto temp = resolveReference(runScope(ast->body, scope)))
            {
                if (temp->GetType() == EObjectType::ReturnValue)
                {
                    if (scope->HasScopeType(ST_Function))
                    {
                        return temp;
                    }
                }
                else if (temp->GetType() == EObjectType::FlowControl)
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
                if (temp->GetType() == EObjectType::ReturnValue)
                {
                    if (scope->HasScopeType(ST_Function))
                    {
                        return temp;
                    }
                }
                else if (temp->GetType() == EObjectType::FlowControl)
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
        case frontend::ENodeType::CreateAndAssign:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::CreateAndAssignNode>(ast))
                {
                    return evalCreateAndAssign(a,scope);
                }
            }
            break;
        case frontend::ENodeType::Function:
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
        case frontend::ENodeType::Call:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::CallNode>(ast))
                {
                    return evalCall(a, scope);
                }
            }
            break;
        case frontend::ENodeType::Scope:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::ScopeNode>(ast))
                {
                    return evalScope(a, scope);
                }
            }
            break;
        case frontend::ENodeType::Return:
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
        case frontend::ENodeType::Throw:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::ThrowNode>(ast))
                {
                    throw makeException(scope,evalExpression(a->expression,scope),ast->debugInfo);
                }
            }
            break;
        case frontend::ENodeType::TryCatch:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::TryCatchNode>(ast))
                {
                    return evalTryCatch(a,scope);
                }
            }
            break;
        case frontend::ENodeType::When:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::WhenNode>(ast))
                {
                    return evalWhen(a, scope);
                }
            }
            break;
        case frontend::ENodeType::For:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::ForNode>(ast))
                {
                    return evalFor(a, scope);
                }
            }
            break;
        case frontend::ENodeType::While:
            {
                if (const auto a = std::dynamic_pointer_cast<frontend::WhileNode>(ast))
                {
                    return evalWhile(a, scope);
                }
            }
            break;
        case frontend::ENodeType::Break:
            return makeFlowControl(FlowControl::Break);
        case frontend::ENodeType::Continue:
            return makeFlowControl(FlowControl::Continue);
        case frontend::ENodeType::Class:
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
        }

        throw makeException(scope,target->ToString(scope) + " is not a dynamic object",ast->debugInfo);
    }

    std::shared_ptr<Object> evalIndex(const std::shared_ptr<frontend::IndexNode>& ast,
                                      const std::shared_ptr<ScopeLike>& scope)
    {
        const auto target = evalExpression(ast->left, scope);
        if (const auto rTarget = cast<DynamicObject>(resolveReference(target)))
        {
            const auto within = evalExpression(ast->within, scope);
            const auto rWithin = resolveReference(within);

            if(auto getResult = rTarget->Get(rWithin, scope))
            {
                return getResult;
            }
            
            throw makeException(scope, rWithin->ToString(scope) + " cannot be used to index " + rTarget->ToString(scope),ast->debugInfo);
        }

        throw makeException(scope,target->ToString(scope) + " is not indexable",ast->debugInfo);
    }

    std::shared_ptr<Object> evalAssign(const std::shared_ptr<frontend::AssignNode>& ast,
                                     const std::shared_ptr<ScopeLike>& scope)
    {
        if(ast->left->type == frontend::ENodeType::Index)
        {
            if(auto asIndex = cast<frontend::IndexNode>(ast->left))
            {
                if(auto target = cast<DynamicObject>(resolveReference(evalExpression(asIndex->left,scope))))
                {
                    auto trueKey = resolveReference(evalExpression(asIndex->within,scope));
                    auto right = evalExpression(ast->value,scope);
                    auto trueVal = resolveReference(right);
                    target->Set(trueKey,trueVal,scope);
                    return right;
                }
            }
        }
        
        if(auto left = evalExpression(ast->left,scope); left->GetType() == EObjectType::Reference)
        {
            auto right = evalExpression(ast->value,scope);
            const auto trueRight = resolveReference(right);
            cast<Reference>(left)->Set(trueRight);
            return right;
        }
        
        throw makeException(scope,"Assign failed",ast->debugInfo);
    }

    std::shared_ptr<Object> evalCreateAndAssign(const std::shared_ptr<frontend::CreateAndAssignNode>& ast,
        const std::shared_ptr<ScopeLike>& scope)
    {
        auto result = evalExpression(ast->value, scope);
        for (auto &identifier : ast->identifiers)
        {
            scope->Assign(identifier, resolveReference(result));
        }
        return result;
    }

    std::shared_ptr<Object> evalTryCatch(const std::shared_ptr<frontend::TryCatchNode>& ast,
                                         const std::shared_ptr<ScopeLike>& scope)
    {
        try
        {
            return runScope(ast->tryScope,scope);
        }
        catch (ExceptionContainer & e)
        {
            auto catchScope = makeScope(scope);
            
            if(!ast->catchArgumentName.empty())
            {
                catchScope->Create(ast->catchArgumentName,e.exception);
            }
            
            return runScope(ast->catchScope,catchScope);
        }
    }

    std::shared_ptr<Module> evalModule(const std::shared_ptr<frontend::ModuleNode>& ast,
        const std::shared_ptr<Program>& program, const std::shared_ptr<ScopeLike>& scope)
    {
        auto mod = makeModule(program);
        
        for (auto& statement : ast->statements)
        {
            evalStatement(statement, scope);
        }

        return mod;
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
        case frontend::ENodeType::Module:
            {
                const auto program = makeProgram();
                return evalModule(std::dynamic_pointer_cast<frontend::ModuleNode>(ast),program);
            }
        case frontend::ENodeType::Function:
            {
                return evalFunction(std::dynamic_pointer_cast<frontend::FunctionNode>(ast), makeScope());
            }
        case frontend::ENodeType::Statement:
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
