#pragma once
#include "batch_matching.h"
#include "../common/stdinc.h"

class CPlugin
{
public:
    CPlugin();
    bool InitSucceeded() const;
    std::filesystem::path GetGameRoot() const;
    std::filesystem::path GetRedirectRoot() const;
    std::filesystem::path GetPluginAsset(const char* rest_path) const;

private:
    bool Init();
    void RegisterPatchSteps(batch_matching& batch_matcher);

    static std::filesystem::path GetModuleFolder(HMODULE m);

    std::filesystem::path m_exe_folder, m_plugin_folder, m_redirect_folder;
    HMODULE m_exe_module, m_plugin_module;
    bool m_init_succeeded = false;
};

extern CPlugin plugin;
