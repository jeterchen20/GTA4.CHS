#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class Translator : public QObject
{
    Q_OBJECT

public:
    Translator(QObject* parent = Q_NULLPTR);

signals:
    void translated_text(QString text);

public slots:
    void translate_text(QString text, QString api_url);

private slots:
    void network_finished(QNetworkReply* reply);

private:
    QNetworkAccessManager _net_manager;
    void SetupNetwork();

    static QJsonObject ParseResult(const QByteArray &data);
};
