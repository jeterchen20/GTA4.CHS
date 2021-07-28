#include "TextEditor.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QCloseEvent>
#include "TextProcess.h"

static const QString APP_TITLE = QStringLiteral("幻梦自由 - GTA4翻译小工具");

TextEditor::TextEditor(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    _saved = true;
    UpdateWindowTitle();

    _table_model = new TextTableModel(this);
    ui.table_texts->setModel(_table_model);

    auto _tokens_layout = new QHBoxLayout;
    ui.widget_tokens->setLayout(_tokens_layout);

    connect(ui.table_texts->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &TextEditor::on_table_texts_currentRowChanged);

    CreateTranslator();
}

TextEditor::~TextEditor()
{
    DestroyTranslator();
}

void TextEditor::log_message(QString text)
{

}

void TextEditor::translated_text(QString text)
{
    ui.line_translated->setText(text);
}

void TextEditor::on_line_translated_textChanged(const QString& text)
{
    _saved = false;
    UpdateWindowTitle();
}

void TextEditor::on_table_texts_currentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
    ChangeLine(current, previous);
}

void TextEditor::on_btn_open_clicked()
{

}

void TextEditor::on_btn_save_clicked()
{
    SaveText();
}

void TextEditor::on_btn_close_clicked()
{
    SaveText();
    _filename.clear();
}

void TextEditor::on_btn_prev_clicked()
{
    auto current_model_index = ui.table_texts->currentIndex();
    auto next_model_index = current_model_index.siblingAtRow(current_model_index.row() - 1);

    ui.table_texts->setCurrentIndex(next_model_index);
}

void TextEditor::on_btn_next_clicked()
{
    auto current_model_index = ui.table_texts->currentIndex();
    auto prev_model_index = current_model_index.siblingAtRow(current_model_index.row() + 1);

    ui.table_texts->setCurrentIndex(prev_model_index);
}

void TextEditor::on_btn_exit_clicked()
{
    QApplication::exit();
}

void TextEditor::on_token_button_clicked()
{
    auto token_string = sender()->property("token").toString();

    //将data插入到光标位置

    if (ui.line_translated->hasFocus())
    {
        //int cur_pos = ui.line_translated->cursorPosition();
        //QString new_text = ui.line_translated->text();
        //
        //ui.line_translated->setText(new_text);
        //ui.line_translated->setCursorPosition(cur_pos + token_string.length());
        ui.line_translated->insert(token_string);
    }
}

void TextEditor::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void TextEditor::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();

    if (urls.isEmpty())
        return;

    LoadText(urls.first().toLocalFile());
}

void TextEditor::closeEvent(QCloseEvent* event)
{

}

void TextEditor::UpdateWindowTitle()
{
    QString result;

    if (_filename.isEmpty())
    {
        result = APP_TITLE;
    }
    else
    {
        if (!_saved)
            result += '*';

        result += APP_TITLE;
        result += '(';
        result += _filename;
        result += ')';
    }

    setWindowTitle(result);
}

void TextEditor::LoadText(const QString& filename)
{
    if (filename.isEmpty())
        return;

    auto texts = TextProcess::ReadText(filename);

    if (!texts.isEmpty())
    {
        _filename = filename;
        _table_model->setTexts(texts);
        _saved = true;
        UpdateWindowTitle();
    }
}

void TextEditor::SaveText()
{
    if (_filename.isEmpty())
        return;

    auto texts = _table_model->getTexts();
    TextProcess::WriteText(texts, _filename);
    _saved = true;
    UpdateWindowTitle();

}

void TextEditor::StoreSingleText(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    _table_model->setData(index.siblingAtColumn(2), ui.line_translated->text(), Qt::DisplayRole);
}

void TextEditor::ChangeLine(const QModelIndex& current, const QModelIndex& previous)
{
    StoreSingleText(previous);

    QString translated = _table_model->data(current.siblingAtColumn(2), Qt::DisplayRole).toString();

    ui.line_original->setText(_table_model->data(current.siblingAtColumn(1), Qt::DisplayRole).toString());
    ui.line_translated->setText(translated);

    //生成Token按钮
    FillTokensWidget(TextProcess::ExtractTokens(translated));

    ui.line_translated->setFocus();
}

void TextEditor::ClearTokenButtons()
{
    _token_buttons.clear();
}

void TextEditor::FillTokensWidget(const QSet<QString>& tokens)
{
    ClearTokenButtons();

    int token_index = 0;

    ui.widget_tokens->layout()->removeItem(ui.tokens_spacer);

    for (auto& token : tokens)
    {
        auto button = new QPushButton;

        button->setProperty("token", token);
        button->setText(QStringLiteral("%1(&%2)").arg(token).arg(token_index + 1));

        connect(button, &QPushButton::clicked, this, &TextEditor::on_token_button_clicked);

        ui.widget_tokens->layout()->addWidget(button);

        _token_buttons.emplace_back(button);

        ++token_index;
    }

    ui.widget_tokens->layout()->addItem(ui.tokens_spacer);
}

void TextEditor::CreateTranslator()
{
    if (_translator != nullptr)
        return;

    _translator = new Translator;

    _translator->moveToThread(&_translate_thread);

    connect(this, &TextEditor::translate_text, _translator, &Translator::translate_text);
    connect(_translator, &Translator::translated_text, this, &TextEditor::translated_text);

    _translate_thread.start();
}

void TextEditor::DestroyTranslator()
{
    _translate_thread.quit();
    _translate_thread.wait();
    delete _translator;
    _translator = nullptr;
}
