#include "file_redirect.h"
#include "plugin.h"
#include "string_util.h"

void file_redirect::scan_files()
{
    m_redirect_table.clear();

    auto redirect_root_path = plugin.GetRedirectRoot();
    auto redirect_root_string = redirect_root_path.string();
    auto game_root_string = plugin.GetGameRoot().string();

    std::filesystem::recursive_directory_iterator file_it{ redirect_root_path };

    for (; file_it != std::filesystem::recursive_directory_iterator{}; ++file_it)
    {
        if (file_it->is_regular_file())
        {
            //扫描到的文件的路径
            auto full_path_string = file_it->path().string();
            auto releative_path_string = full_path_string.substr(game_root_string.size() + 1);

            //要替代的游戏文件的路径
            auto substitue_relative_path_string = full_path_string.substr(redirect_root_string.size() + 1);
            auto substitue_full_path_string = game_root_string + full_path_string.substr(redirect_root_string.size());

            //windows不区分大小写
            m_redirect_table.emplace(string_util::hash_string(substitue_full_path_string, false), full_path_string);

            //TODO: 研究用releative_path_string不行的原因
            m_redirect_table.emplace(string_util::hash_string(substitue_relative_path_string, false), full_path_string);
        }
    }
}

const char* file_redirect::redirect_path(const char* original_path) const
{
    auto table_it = m_redirect_table.find(string_util::hash_string(original_path, false));

    return
        table_it == m_redirect_table.end() ?
        original_path :
        table_it->second.c_str();
}
