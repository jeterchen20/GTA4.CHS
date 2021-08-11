#pragma once
#include "../common/stdinc.h"

class CTable
{
public:
    void LoadTable(const std::filesystem::path& filename);
    CharacterPos GetCharPos(GTAChar chr) const;

private:
    std::unordered_map<uint, CharacterPos> m_Table;
};

extern CTable GlobalTable;
