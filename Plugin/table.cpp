#include "table.h"

void CTable::LoadTable(const std::filesystem::path& filename)
{
    std::vector<CharacterData> buffer;

    BinaryFile file(filename,"rb");

    m_Table.clear();
    file.Seek(0, SEEK_END);
    auto size = file.Tell();
    file.Seek(0, SEEK_SET);
    file.ReadArray(size / sizeof(CharacterData), buffer);

    m_Table.reserve(buffer.size() * 2);

    for (auto& entry : buffer)
    {
        m_Table.emplace(entry.code, entry.pos);
    }
}

CharacterPos CTable::GetCharPos(GTAChar chr) const
{
    auto it = m_Table.find(chr);

    if (it == m_Table.end())
    {
        return CharacterPos{ 50,63 }; //字库中此位置绘制白色方块
    }
    else
    {
        return it->second;
    }
}
