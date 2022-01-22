#include "HTMLDocument.h"

void CHtmlDocument::ReadDocument(MemoryFile& file)
{
    m_pRootElement.read_object(file);
    m_pBody.read_object(file);
    m_pszTitle.read_object(file);
    m_pTxd.read_object(file);
    _f10.read_object(file);
    m_childNodes.read_object(file);
    m_pStylesheet.read_object(file);
    file.Read(&pad, 4);
}

void CHtmlDocument::WriteDocument(MemoryFile& file)
{

}

std::vector<std::string*> CHtmlDocument::GetStringList()
{
    std::vector<std::string*> result;

    result.push_back(&m_pszTitle.data);

    return result;
}
