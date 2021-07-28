#pragma once
#include <QSet>
#include <QString>
#include "Text.h"

struct TextMeta
{
    int token_char_count = 0;
    int token_count = 0;
    QVector<int> token_char_indexes;
    QSet<QString> tokens;
};

class TextProcess
{
public:
    static TextMeta AnalyseText(const QString& text);
    static bool CompareTokens(const QString& text1, const QString& text2);

    static QVector<Text> ReadText(const QString& filename);
    static void WriteText(const QVector<Text>& texts, const QString& filename);
};
