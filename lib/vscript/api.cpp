#include "vscript/api.hpp"
#define NOMINMAX
#include <windows.h>

namespace vs::api
{
    
    frontend::TSmartPtrType<frontend::Module> importNative(const std::filesystem::path& path,frontend::TSmartPtrType<frontend::Program>& program)
    {
        const HINSTANCE dll = LoadLibrary(path.string().c_str());

        if(!dll)
        {
            throw std::runtime_error("Failed to load the native module");
        }

        const auto importFn = (_vs_import)GetProcAddress(dll,"_vs_import");

        if(!importFn)
        {
            FreeLibrary(dll);
            throw std::runtime_error("Failed to find entry in native module");
        }

        frontend::TSmartPtrType<frontend::Module> mod;
        importFn(mod,program);

        return mod;
    }
}
