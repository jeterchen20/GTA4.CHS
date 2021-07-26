#include "plugin.h"
#include "byte_pattern.h"
#include "logger.h"

BOOL WINAPI DllMain(HMODULE module, DWORD reason, void *reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        byte_pattern::start_log("gta4.chs");
        Logger::StartLog("GTA4.CHS.log");

        if (!PluginObject.Init(module))
        {
            return FALSE;
        }
    }

    return TRUE;
}
