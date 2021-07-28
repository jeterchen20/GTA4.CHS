#pragma once
#include <QSet>
#include <QString>
#include "Text.h"

class TextProcess
{
public:
    static QSet<QString> ExtractTokens(const QString& text);
    static bool CompareTokens(const QString& text1, const QString& text2);
    static bool ValidateTokens(const QString& text);
    static QVector<Text> ReadText(const QString& filename);
    static void WriteText(const QVector<Text>& texts, const QString& filename);
};
