#include "TextTableModel.h"

TextTableModel::TextTableModel(QObject* parent)
    :QAbstractTableModel(parent)
{
}

QVariant TextTableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return _texts[index.row()].hash;

        case 1:
            return _texts[index.row()].original;

        case 2:
            return _texts[index.row()].translated;

        default:
            break;
        }
    }

    return QVariant();
}

QVariant TextTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0:
                return QStringLiteral("哈希值");

            case 1:
                return QStringLiteral("原文");

            case 2:
                return QStringLiteral("译文");

            default:
                break;
            }
        }

        if (orientation == Qt::Vertical)
        {
            return section + 1;
        }
    }

    return QVariant();
}

int TextTableModel::rowCount(const QModelIndex& parent) const
{
    return _texts.size();
}

int TextTableModel::columnCount(const QModelIndex& parent) const
{
    return 3;
}

bool TextTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::DisplayRole)
    {
        _texts[index.row()].translated = value.toString();
        emit dataChanged(index.siblingAtColumn(2), index.siblingAtColumn(2));
        return true;
    }

    return false;
}

void TextTableModel::setTexts(const QVector<Text>& texts)
{
    beginResetModel();
    _texts = texts;
    endResetModel();
}

QVector<Text> TextTableModel::getTexts() const
{
    return _texts;
}
