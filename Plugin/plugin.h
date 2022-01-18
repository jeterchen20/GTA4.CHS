#pragma once
#include "../common/stdinc.h"
#include "batch_matching.h"
#include "file_redirect.h"
#include "table.h"
#include "game.h"

HANDLE WINAPI RedirectCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

class CPlugin
{
public:
    CPlugin();
    bool InitSucceeded() const;
    std::filesystem::path GetGameRoot() const;
    std::filesystem::path GetRedirectRoot() const;
    std::filesystem::path GetPluginAsset(const char* rest_path) const;

    //保证进入构造函数之后这些对象已经初始化了
    file_redirect redirector;
    CTable table;
    CGame game;

private:
    bool Init();
    void RegisterPatchSteps(batch_matching& batch_matcher);

    static std::filesystem::path GetModuleFolder(HMODULE m);

    std::filesystem::path m_exe_folder, m_plugin_folder, m_redirect_folder;
    HMODULE m_exe_module, m_plugin_module;
    bool m_init_succeeded = false;
};

extern CPlugin plugin;
