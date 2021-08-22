#include "IVText.h"

void IVText::Process0Arg()
{
    wchar_t temp[512];

    HMODULE self = GetModuleHandleW(NULL);
    GetModuleFileNameW(self, temp, 512);

    PathType text_path = temp;
    text_path = text_path.parent_path();

    m_Data.clear();
    m_Collection.clear();

    LoadText(text_path / "GTA4.txt");

    if (!m_Data.empty())
    {
        GenerateBinary(text_path / "chinese.gxt");
        GenerateCollection(text_path / "characters.txt");
        GenerateTable(text_path / "table.dat");
    }
}

void IVText::Process2Args(const PathType& arg1, const PathType& arg2)
{
    m_Data.clear();
    m_Collection.clear();

    if (is_directory(arg1))
    {
        create_directories(arg2);
        LoadTexts(arg1);
        GenerateBinary(arg2 / "chinese.gxt");
        GenerateCollection(arg2 / "characters.txt");
        GenerateTable(arg2 / "table.dat");
    }
    else if (is_regular_file(arg1))
    {
        create_directories(arg2);
        LoadBinary(arg1);
        GenerateTexts(arg2);
    }
}

void IVText::SkipUTF8Signature(std::ifstream& stream)
{
    char bom[3];

    if (stream.get(bom[0]) && stream.get(bom[1]) && stream.get(bom[2]))
    {
        if (bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')
        {
            return;
        }
    }

    stream.seekg(0);
}

bool IVText::IsNativeCharacter(char32_t character)
{
    return (character < 0x100 || character == L'™');
}

void IVText::CollectCharacters(const tiny_utf8::utf8_string& text)
{
    for (auto character : text)
    {
        if (!IsNativeCharacter(character))
        {
            m_Collection.insert(character);
        }
    }
}

void IVText::LoadText(const PathType& input_text)
{
    std::regex table_regex(R"(\[([0-9a-zA-Z_]{1,7})\])");
    std::regex entry_regex(R"((0[xX][0-9a-fA-F]{8})=(.*))");
    std::regex origin_regex(R"(;(0[xX][0-9a-fA-F]{8})=(.*))");

    std::smatch matches;

    std::string line;

    auto table_iter = m_Data.end();
    std::string filename = input_text.filename().string();

    size_t line_no = 0;

    std::ifstream stream(input_text);

    if (!stream)
    {
        std::cout << "打开文件 " << input_text.string() << " 失败。" << std::endl;
        return;
    }

    SkipUTF8Signature(stream);

    while (std::getline(stream, line))
    {
        ++line_no;

        line.erase(0, line.find_first_not_of(' '));
        line.erase(line.find_last_not_of(' ') + 1);

        if (line.empty() || line.front() == ';')
        {

        }
        else if (line.front() == '0' && regex_match(line, matches, entry_regex))
        {
            if (table_iter != m_Data.end())
            {
                uint32_t hash = stoul(matches.str(1), nullptr, 16);
                tiny_utf8::utf8_string text = matches.str(2);

                CollectCharacters(text);

                table_iter->second.emplace_back(hash, text);
            }
            else
            {
                std::cout << filename << ": " << "第" << line_no << "行没有所属的表。" << std::endl;
            }
        }
        else if (line.front() == '[' && regex_match(line, matches, table_regex))
        {
            table_iter = m_Data.emplace(matches.str(1), std::vector<EntryType>()).first;
        }
        else
        {
            std::cout << filename << ": " << "第" << line_no << "行无法识别。" << std::endl;
        }
    }
}

void IVText::LoadTexts(const PathType& input_texts)
{
    std::filesystem::directory_iterator dir_it{ input_texts };

    while (dir_it != std::filesystem::directory_iterator{})
    {
        PathType filename = dir_it->path();

        LoadText(filename);

        ++dir_it;
    }
}

void IVText::GenerateBinary(const PathType& output_binary) const
{
    BinaryFile file(output_binary, BinaryFile::OpenMode::WriteOnly);

    std::vector<uint8_t> buffer;
    long writePostion;

    GXTHeader gxtHeader;

    TableBlock tableBlock;
    KeyBlockOthers keyBlock;

    DataBlock dataBlock;

    TableEntry tableEntry;
    KeyEntry keyEntry;

    std::vector<TableEntry> tables;
    std::vector<KeyEntry> keys;
    std::vector<uint16_t> datas;

    if (!file)
    {
        std::cout << "创建输出文件 " << output_binary.string() << " 失败。" << std::endl;
        return;
    }

    gxtHeader.Version = 4;
    gxtHeader.CharBits = 16;
    file.Write(gxtHeader);

    tableBlock.TABL[0] = 'T';
    tableBlock.TABL[1] = 'A';
    tableBlock.TABL[2] = 'B';
    tableBlock.TABL[3] = 'L';

    keyBlock.Body.TKEY[0] = 'T';
    keyBlock.Body.TKEY[1] = 'K';
    keyBlock.Body.TKEY[2] = 'E';
    keyBlock.Body.TKEY[3] = 'Y';

    dataBlock.TDAT[0] = 'T';
    dataBlock.TDAT[1] = 'D';
    dataBlock.TDAT[2] = 'A';
    dataBlock.TDAT[3] = 'T';

    tableBlock.Size = m_Data.size() * sizeof(TableEntry);
    file.Write(tableBlock);

    writePostion = 4 + 8 + tableBlock.Size;

    tables.clear();

    for (auto& table : m_Data)
    {
        keys.clear();
        datas.clear();

        strcpy(tableEntry.Name, table.first.c_str());
        tableEntry.Offset = static_cast<int>(writePostion);
        tables.push_back(tableEntry);

        strcpy(keyBlock.Name, table.first.c_str());
        keyBlock.Body.Size = table.second.size() * sizeof(KeyEntry);

        for (auto& key : table.second)
        {
            keyEntry.Hash = key.first;
            keyEntry.Offset = datas.size() * 2;

            auto wideText = key.second;
            LiteralToGame(wideText);

            std::copy(wideText.begin(), wideText.end(), back_inserter(datas));
            datas.push_back(0);

            keys.push_back(keyEntry);
        }

        dataBlock.Size = datas.size() * 2;

        file.Seek(writePostion, BinaryFile::SeekMode::Begin);

        if (table.first == "MAIN")
        {
            file.Write(keyBlock.Body);
        }
        else
        {
            file.Write(keyBlock);
        }

        file.WriteArray(keys);
        file.Write(dataBlock);
        file.WriteArray(datas);

        writePostion = file.Tell();
    }

    file.Seek(4 + 8, BinaryFile::SeekMode::Begin);
    file.WriteArray(tables);
}

void IVText::GenerateCollection(const PathType& output_text) const
{
    size_t count = 0;
    tiny_utf8::utf8_string u8_text;

    for (auto character : m_Collection)
    {
        if (count == 64)
        {
            u8_text += L'\n';
            count = 0;
        }

        u8_text += character;

        ++count;
    }

    std::ofstream stream(output_text, std::ios::trunc);

    std::copy(u8_text.c_str(), u8_text.c_str() + u8_text.size(), std::ostreambuf_iterator<char>(stream));
}

void IVText::GenerateTable(const PathType& output_binary) const
{
    std::vector<CharacterData> datas;

    CharacterData data;

    data.pos.row = 0;
    data.pos.column = 0;

    for (auto chr : m_Collection)
    {
        data.code = chr;
        if (data.pos.column == 64)
        {
            ++data.pos.row;
            data.pos.column = 0;
        }

        datas.push_back(data);

        ++data.pos.column;
    }

    BinaryFile stream(output_binary, BinaryFile::OpenMode::WriteOnly);
    stream.WriteArray(datas);
}

void IVText::FixCharacters(tiny_utf8::utf8_string& wtext)
{
    //bad character in IV stock text: 0x85 0x92 0x94 0x96 0x97 0xA0
    //bad character in EFLC stock text: 0x93

    for (auto& character : wtext)
    {
        switch (character)
        {
        case 0x85:
            character = L' ';
            break;

        case 0x92:
        case 0x94:
            character = L'\'';
            break;

        case 0x93: //EFLC
            break;

        case 0x96:
            character = L'-';
            break;

        case 0x97:
        case 0xA0:
            character = L' ';
            break;

        default:
            break;
        }
    }
}

void IVText::LiteralToGame(tiny_utf8::utf8_string& wtext)
{
    for (auto& character : wtext)
    {
        switch (character)
        {
        case L'™':
            character = 0x99;
            break;

        default:
            break;
        }
    }
}

void IVText::GameToLiteral(tiny_utf8::utf8_string& wtext)
{
    for (auto& character : wtext)
    {
        switch (character)
        {
        case 0x99:
            character = L'™';
            break;

        default:
            break;
        }
    }
}

void IVText::LoadBinary(const PathType& input_binary)
{
    GXTHeader gxtHeader;
    TableBlock tableBlock;
    KeyBlockOthers keyBlock;
    DataBlock tdatHeader;

    std::vector<TableEntry> tables;
    std::vector<KeyEntry> keys;
    std::vector<CharType> datas;

    m_Data.clear();

    auto tableIter = m_Data.end();

    BinaryFile file(input_binary, BinaryFile::OpenMode::ReadOnly);

    if (!file)
    {
        std::cout << "打开输入文件 " << input_binary.string() << " 失败。" << std::endl;
        return;
    }

    file.Read(gxtHeader);

    file.Read(tableBlock);

    file.ReadArray(tableBlock.Size / sizeof(TableEntry), tables);

    for (TableEntry& table : tables)
    {
        tableIter = m_Data.emplace(table.Name, std::vector<EntryType>()).first;

        file.Seek(table.Offset, BinaryFile::SeekMode::Begin);

        if (strcmp(table.Name, "MAIN") != 0)
        {
            file.Read(keyBlock);
        }
        else
        {
            file.Read(keyBlock.Body);
        }

        file.ReadArray(keyBlock.Body.Size / sizeof(KeyEntry), keys);

        file.Read(tdatHeader);

        file.ReadArray(tdatHeader.Size / 2, datas);

        for (auto& key : keys)
        {
            tiny_utf8::utf8_string wtext;
            auto offset = key.Offset / 2;

            while (datas[offset] != 0)
            {
                wtext += datas[offset];
                ++offset;
            }

            FixCharacters(wtext);
            GameToLiteral(wtext);

            tableIter->second.emplace_back(key.Hash, wtext);
        }
    }
}

void IVText::GenerateTexts(const PathType& output_texts) const
{
    std::ofstream stream;
    std::string line;

    for (auto& table : m_Data)
    {
        stream.open(output_texts / (table.first + ".txt"), std::ios::trunc);

        if (!stream)
        {
            std::cout << "创建输出文件失败" << std::endl;
        }

        stream.write("\xEF\xBB\xBF", 3);

        stream << fmt::sprintf("[%s]\n", table.first);

        for (auto& entry : table.second)
        {
            line = fmt::sprintf("0x%08X=%s\n", entry.first, entry.second.cpp_str());
            stream << ';' << line << line << '\n';
        }

        stream.close();
    }
}
