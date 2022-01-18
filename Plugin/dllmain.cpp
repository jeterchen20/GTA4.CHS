#include "plugin.h"
#include "byte_pattern.h"

BOOL WINAPI DllMain(HMODULE module, DWORD reason, void *reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        return plugin.InitSucceeded() ? TRUE : FALSE;
    }

    return TRUE;
}
