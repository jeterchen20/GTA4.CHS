#pragma once
#include <QAbstractTableModel>
#include "Text.h"

class TextTableModel : public QAbstractTableModel
{
public:
    TextTableModel(QObject* parent = Q_NULLPTR);

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    void setTexts(const QVector<Text> &texts);
    QVector<Text> getTexts() const;

private:
    QVector<Text> _texts;

    QModelIndex _current_index;
};
