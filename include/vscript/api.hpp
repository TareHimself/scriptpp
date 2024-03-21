#pragma once
#include <filesystem>
#include "vscript/runtime/Module.hpp"
#include "vscript/runtime/Object.hpp"

namespace vs::api
{
    typedef void (__stdcall* _vs_import)(vs::runtime::TSmartPtrType<runtime::Module>& mod,vs::runtime::TSmartPtrType<runtime::ProgramScope>& scope);

#define VS_API_MODULE(name,func) \
    auto _create_fn = ##func; \
    extern "C" __declspec(dllexport) void _vs_import(vs::runtime::TSmartPtrType<vs::runtime::Module>& mod,vs::runtime::TSmartPtrType<vs::runtime::ProgramScope>& scope) \
    { \
        mod = _create_fn(scope); \
    }


    runtime::TSmartPtrType<runtime::Module> importNative(const std::filesystem::path& path,runtime::TSmartPtrType<runtime::ProgramScope>& program);
}
