#pragma once
#include "../common/stdinc.h"

HANDLE WINAPI RedirectCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                  DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

class file_redirect
{
public:
    void scan_files(); //扫描目录下的文件，在redirect_path()中此目录被当作gtaiv.exe所在位置

    //.../GTAIV/pc/html/123.dat
    //会被替换成
    //.../GTAIV/plugins/GTA4.CHS/redirect/pc/html/123.dat
    const char* redirect_path(const char* original_path) const;

private:
    std::unordered_map<std::size_t, std::string> m_redirect_table;
};

extern file_redirect redirector;
