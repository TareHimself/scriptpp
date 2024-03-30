#pragma once
#include <filesystem>
#include "frontend\frontend.hpp"

namespace vs::api
{
    typedef void (__stdcall* _vs_import)(std::shared_ptr<frontend::Module>& mod,std::shared_ptr<frontend::Program>& scope);

#define VS_API_MODULE(name,func) \
    auto _create_fn = ##func; \
    extern "C" __declspec(dllexport) void _vs_import(std::shared_ptr<vs::frontend::Module>& mod,std::shared_ptr<vs::frontend::Program>& program) \
    { \
        mod = _create_fn(program); \
    }


    std::shared_ptr<frontend::Module> importNative(const std::filesystem::path& path,std::shared_ptr<frontend::Program>& program);
}
