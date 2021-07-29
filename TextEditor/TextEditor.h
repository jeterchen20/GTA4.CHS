#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TextEditor.h"

#include "TextTableModel.h"

#include "Translator.h"
#include <QThread>
#include <vector>
#include <memory>

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    TextEditor(QWidget* parent = Q_NULLPTR);
    ~TextEditor();

signals:
    void translate_text(QString text, QString api_url);

public slots:
    void log_message(QString text);

private slots:
    void machine_translated_text(QString text);

    void on_line_translated_textChanged(const QString &text);
    void on_table_texts_currentRowChanged(const QModelIndex& current, const QModelIndex& previous);
    void on_btn_open_clicked();
    void on_btn_save_clicked();
    void on_btn_close_clicked();
    void on_btn_prev_clicked();
    void on_btn_next_clicked();
    void on_btn_exit_clicked();

    void on_token_button_clicked();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::TextEditorClass ui;
    TextTableModel* _table_model;

    void UpdateWindowTitle();

    QString _filename;
    bool _saved;
    void LoadText(const QString& filename);
    void SaveText();
    void StoreSingleText(const QModelIndex &index);
    void ChangeLine(const QModelIndex& current, const QModelIndex& previous);

    std::vector<std::unique_ptr<QPushButton>> _token_buttons;
    void ClearTokenButtons();
    void FillTokensWidget(const QSet<QString>& tokens);

    Translator* _translator = nullptr;
    QThread _translate_thread;
    void CreateTranslator();
    void DestroyTranslator();
};
