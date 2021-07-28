#include "TextProcess.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

TextMeta TextProcess::AnalyseText(const QString& text)
{
    TextMeta result;

    int token_index = text.indexOf('~');

    while (token_index != -1)
    {
        result.token_char_indexes += token_index;
        token_index = text.indexOf('~', token_index + 1);
    }

    result.token_char_count = result.token_char_indexes.size();
    result.token_count = result.token_char_indexes.size() / 2;

    for (token_index = 0; token_index < result.token_count; ++token_index)
    {
        result.tokens += text.mid(result.token_char_indexes[token_index * 2],
            result.token_char_indexes[token_index * 2 + 1] - result.token_char_indexes[token_index * 2] + 1);
    }

    return result;
}

bool TextProcess::CompareTokens(const QString& text1, const QString& text2)
{
    return false;
}

QVector<Text> TextProcess::ReadText(const QString& filename)
{
    QHash<QString, int> hash_index_table; //某个hash在result中的index
    QVector<Text> result;
    QRegularExpression text_regex(R"((0[xX][0-9a-fA-F]{8})=(.*))");

    QRegularExpressionMatch matches;

    QFile file;
    QTextStream stream;
    QString line;

    int line_no = 0;

    file.setFileName(filename);

    if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        return result;
    }

    stream.setDevice(&file);

    while (stream.readLineInto(&line))
    {
        QRegularExpressionMatch match_result;
        Text text_entry;

        ++line_no;

        if (line.isEmpty() || line.front() == '[')
        {
            continue;
        }

        line = line.trimmed();

        if (line.front() == '0')
        {
            match_result = text_regex.match(line, 0);
        }
        else if (line.front() == ';')
        {
            match_result = text_regex.match(line, 1);
        }

        if (!match_result.hasMatch())
            continue;

        QString hash = match_result.captured(1);
        QString text = match_result.captured(2);
        int hash_index;

        auto hash_it = hash_index_table.find(hash);

        if (hash_it == hash_index_table.end())
        {
            hash_index = result.size();
            hash_index_table.insert(hash, hash_index);
            result.push_back({});
        }
        else
        {
            hash_index = hash_it.value();
        }

        result[hash_index].hash = hash;

        if (line.front() == '0')
        {
            result[hash_index].translated = text;
        }
        else if (line.front() == ';')
        {
            result[hash_index].original = text;
        }
    }

    return result;
}

void TextProcess::WriteText(const QVector<Text>& texts, const QString& filename)
{
    QFile file;
    QTextStream stream;

    file.setFileName(filename);

    if (!file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return;
    }

    stream.setDevice(&file);
    stream.setCodec("UTF-8");
    stream.setGenerateByteOrderMark(true);

    for (auto& text : texts)
    {
        stream << ';' << text.hash << '=' << text.original << '\n';
        stream << text.hash << '=' << text.translated << '\n';
        stream << '\n';
    }
}
