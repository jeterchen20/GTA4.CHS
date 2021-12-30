#include "plugin.h"
#include "byte_pattern.h"

BOOL WINAPI DllMain(HMODULE module, DWORD reason, void *reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        if (!CPlugin::Init(module))
        {
            return FALSE;
        }
    }

    return TRUE;
}
