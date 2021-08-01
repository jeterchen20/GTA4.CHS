#pragma once
#include "../common/stdinc.h"

class CCharacterTable
{
public:
    void LoadTable(const std::filesystem::path& filename);
    CharacterPos GetCharPos(GTAChar chr) const;

    static CCharacterTable GlobalTable;

private:
    std::unordered_map<uint, CharacterPos> m_Table;
};
