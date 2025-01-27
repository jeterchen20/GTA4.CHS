﻿#pragma once
#include "../common/stdinc.h"

enum class game_episode
{
    GAME_EPISODE_LC,
    GAME_EPISODE_LOST,
    GAME_EPISODE_TONY
};

class file_redirect
{
public:
    void scan_files(); //扫描目录下的文件，在redirect_path()中此目录被当作gtaiv.exe所在位置

    //...pc/html/123.dat
    //会被替换成
    //...plugins/GTA4.CHS/redirect/pc/html/123.dat
    const char* redirect_path(const char* original_path, game_episode episode = game_episode::GAME_EPISODE_LC) const;

private:
    std::unordered_map<std::size_t, std::string> m_redirect_table;
};
