#include "WHMTool.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QtZlib/zlib.h>
#include "pgBase.h"

QByteArray inflate_array(QByteArray& compressed, uint uncompressed_size)
{
    QByteArray uncompressed;
    uncompressed.resize(uncompressed_size);
    ulong dest_size = uncompressed_size;

    //估计是从header算出SystemMemorySize+GraphicsMemorySize作为解压后数据大小
    uncompress(reinterpret_cast<unsigned char*>(uncompressed.data()), &dest_size, reinterpret_cast<unsigned char*>(compressed.data()), compressed.size());

    return uncompressed;
}

void uncompress_rsc(const QString& filename)
{
    QFile file;

    file.setFileName(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;

    RSCHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(RSCHeader));

    uint uncompressed_size = ((header.flags & 0x7FF) << (((header.flags >> 11) & 0xF) + 8)) +
        (((header.flags >> 15) & 0x7FF) << (((header.flags >> 26) & 0xF) + 8));

    QByteArray compressed = file.readAll();
    QByteArray uncompressed = inflate_array(compressed, uncompressed_size);

    file.setFileName(filename + ".uncompressed");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    file.write(uncompressed);
}

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
