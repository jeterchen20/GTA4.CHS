#include "Translator.h"

Translator::Translator(QObject* parent)
    :QObject(parent)
{
    SetupNetwork();
}

void Translator::translate_text(QString text, QString api_url)
{

}

void Translator::network_finished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
        return;

    QJsonObject json = ParseResult(reply->readAll());
}

void Translator::SetupNetwork()
{
    connect(&_net_manager, &QNetworkAccessManager::finished, this, &Translator::network_finished);
}

QJsonObject Translator::ParseResult(const QByteArray& data)
{
    return {};
}
