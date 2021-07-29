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
    void machine_translated_text(QString text);

public slots:
    void translate_text(QString text);

private slots:
    void network_finished(QNetworkReply* reply);

private:
    QNetworkAccessManager _net_manager;
    void SetupNetwork();

    static QJsonObject ParseResult(const QByteArray &data);
};
