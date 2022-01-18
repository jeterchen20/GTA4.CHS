#include "file_redirect.h"
#include "plugin.h"
#include "string_util.h"

file_redirect redirector;

HANDLE __stdcall RedirectCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
    return CreateFileA(redirector.redirect_path(lpFileName), dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

void file_redirect::scan_files()
{
    m_redirect_table.clear();

    auto redirect_root = plugin.GetRedirectRoot();
    auto redirect_root_string = redirect_root.string();
    auto game_root_string = plugin.GetGameRoot().string();

    std::filesystem::recursive_directory_iterator file_it{ redirect_root };

    for (; file_it != std::filesystem::recursive_directory_iterator{}; ++file_it)
    {
        if (file_it->is_regular_file())
        {
            //扫描到的文件的完整路径
            auto full_path_string = file_it->path().string();

            //要替代的游戏文件的完整路径
            auto virtual_game_path_string = game_root_string + full_path_string.substr(redirect_root_string.size());

            m_redirect_table.emplace(string_util::hash_string(virtual_game_path_string), full_path_string);
        }
    }
}

const char* file_redirect::redirect_path(const char* original_path) const
{
    auto table_it = m_redirect_table.find(string_util::hash_string(original_path));

    return
        table_it == m_redirect_table.end() ?
        original_path :
        table_it->second.c_str();
}
