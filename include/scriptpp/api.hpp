#pragma once
#include <filesystem>
#include "runtime/runtime.hpp"

namespace spp::api
{
    typedef void (__stdcall* _vs_import)(std::shared_ptr<runtime::Module>& mod,std::shared_ptr<runtime::Program>& scope);

#define SPP_API_MODULE(name,func) \
    auto _create_fn = ##func; \
    extern "C" __declspec(dllexport) void _vs_import(std::shared_ptr<spp::runtime::Module>& mod,std::shared_ptr<spp::runtime::Program>& program) \
    { \
        mod = _create_fn(program); \
    }


    std::shared_ptr<runtime::Module> importNative(const std::filesystem::path& path,std::shared_ptr<runtime::Program>& program);
}
