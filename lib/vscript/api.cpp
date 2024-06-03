#include "vscript/api.hpp"
#define NOMINMAX
#include <windows.h>

namespace vs::api
{
    
    std::shared_ptr<runtime::Module> importNative(const std::filesystem::path& path,std::shared_ptr<runtime::Program>& program)
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

        std::shared_ptr<runtime::Module> mod;
        importFn(mod,program);

        return mod;
    }
}
