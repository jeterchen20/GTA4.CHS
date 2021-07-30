#include "WHMTool.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QtZlib/zlib.h>

struct RscHeader {
    quint32   dwMagic;
    quint32   dwVersion;
    quint32   dwFlags;
};

WHMTool::WHMTool(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void WHMTool::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void WHMTool::dropEvent(QDropEvent* event)
{
    auto urls = event->mimeData()->urls();

    if (urls.isEmpty())
        return;

    uncompress_rsc(urls.first().toLocalFile());
}
