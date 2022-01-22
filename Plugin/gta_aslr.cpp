#include "gta_aslr.h"

static std::once_flag get_module_base_flag;
static std::uintptr_t module_base;

static void get_module_base()
{
    module_base = reinterpret_cast<std::uintptr_t>(::GetModuleHandleW(NULL));
}

namespace util
{
    std::uintptr_t gta_aslr(std::uintptr_t address)
    {
        std::call_once(get_module_base_flag, get_module_base);

        //address是ida里得到的地址，基址是0x400000
        return (address - 0x400000u) + module_base;
    }
}
