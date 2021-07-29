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
    static QString ValidateText(const QString& translated, const QString& origin);
    static QVector<Text> ReadText(const QString& filename);
    static void WriteText(const QVector<Text>& texts, const QString& filename);
};
