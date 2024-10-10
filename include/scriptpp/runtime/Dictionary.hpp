#pragma once
#include "DynamicObject.hpp"
#include "Prototype.hpp"

namespace spp::runtime
{
    class DictionaryPrototype;
    
    class Dictionary : public DynamicObject
    {
        std::unordered_map<std::shared_ptr<Object>,std::shared_ptr<Object>> _entries{};
    public:
        Dictionary();
        
        Dictionary(const std::unordered_map<std::shared_ptr<Object>,std::shared_ptr<Object>>& data);
        
        static std::shared_ptr<DictionaryPrototype> Prototype;
        
        void Init() override;

        std::shared_ptr<Object> PutItem(const std::shared_ptr<FunctionScope>& fnScope);

        std::shared_ptr<Object> GetItem(const std::shared_ptr<FunctionScope>& fnScope);
        
        std::shared_ptr<Object> HasItem(const std::shared_ptr<FunctionScope>& fnScope);

        std::shared_ptr<Object> Get(const std::shared_ptr<Object>& key, const std::shared_ptr<ScopeLike>& scope) const override;

        std::string ToString(const std::shared_ptr<ScopeLike>&) const override;
    };
    
    std::shared_ptr<Dictionary> makeDictionary(const std::unordered_map<std::shared_ptr<Object>,std::shared_ptr<Object>>& data);
    std::shared_ptr<Dictionary> makeDictionary(const std::unordered_map<std::string,std::shared_ptr<Object>>& data);
    std::shared_ptr<Dictionary> makeDictionary();

    class DictionaryPrototype : public Prototype
    {
    public:
        DictionaryPrototype();

        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;

        std::shared_ptr<DynamicObject> CreateInstance(std::shared_ptr<FunctionScope>& scope) override;

        std::string GetName() const override;
    };
}
