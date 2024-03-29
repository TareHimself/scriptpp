#pragma once
#include <filesystem>
#include "frontend\frontend.hpp"

namespace vs::api
{
    typedef void (__stdcall* _vs_import)(vs::frontend::TSmartPtrType<frontend::Module>& mod,vs::frontend::TSmartPtrType<frontend::Program>& scope);

#define VS_API_MODULE(name,func) \
    auto _create_fn = ##func; \
    extern "C" __declspec(dllexport) void _vs_import(vs::frontend::TSmartPtrType<vs::frontend::Module>& mod,vs::frontend::TSmartPtrType<vs::frontend::Program>& scope) \
    { \
        mod = _create_fn(scope); \
    }


    frontend::TSmartPtrType<frontend::Module> importNative(const std::filesystem::path& path,frontend::TSmartPtrType<frontend::Program>& program);
}
