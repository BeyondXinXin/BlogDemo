#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QPair>
#include <QUrl>
#include <QVector>

struct NetworkReply
{
    QNetworkReply::NetworkError error = QNetworkReply::HostNotFoundError;
    QByteArray data;

    QString ErrorStr();
};

class NetworkAccess : public QObject
{
    Q_OBJECT
public:
    typedef QVector<QPair<QByteArray, QByteArray>> RawHeaderPairs;

    explicit NetworkAccess(QObject *p_parent = nullptr);

    void AsyncRequest(const QUrl &url);

    static NetworkReply Request(const QUrl &url);

    static NetworkReply Request(const QUrl &url, const RawHeaderPairs &raw_header);

    static NetworkReply Put(const QUrl &url, const RawHeaderPairs &raw_header, const QByteArray &data);

    static NetworkReply Post(const QUrl &url, const RawHeaderPairs &raw_header, const QByteArray &data);

    static NetworkReply Delete(const QUrl &url, const RawHeaderPairs &raw_header, const QByteArray &data);

Q_SIGNALS:
    void SgnRequestFinished(const NetworkReply &reply, const QString &url);

private:
    static void HandleReply(QNetworkReply *reply, NetworkReply &m_reply);
    static QNetworkRequest NetworkRequest(const QUrl &url);
    static NetworkReply SendRequest(const QUrl &url,
                                    const RawHeaderPairs &header,
                                    const QByteArray &action,
                                    const QByteArray &data);

private:
    QNetworkAccessManager net_access_mgr_;
};

#endif // NETWORKACCESS_H
