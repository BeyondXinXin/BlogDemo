#include "network_access.h"

#include <QEventLoop>
#include <QMetaEnum>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QThread>

QString NetworkReply::ErrorStr()
{
    static const auto index_enum = QNetworkReply::staticMetaObject.indexOfEnumerator("NetworkError");
    const auto meta_enum = QNetworkReply::staticMetaObject.enumerator(index_enum);
    return meta_enum.key(error);
}

NetworkAccess::NetworkAccess(QObject * p_parent)
  : QObject(p_parent)
{
    connect(&net_access_mgr_, &QNetworkAccessManager::finished,
            this, [this](QNetworkReply * p_reply) {
                NetworkReply reply;
                NetworkAccess::HandleReply(p_reply, reply);
                emit SgnRequestFinished(reply, p_reply->request().url().toString());
            });
}

void NetworkAccess::AsyncRequest(const QUrl & url)
{
    if (!url.isValid()) {
        return;
    }
    net_access_mgr_.get(NetworkRequest(url));
}

NetworkReply NetworkAccess::Request(const QUrl & url)
{
    return Request(url, RawHeaderPairs());
}

NetworkReply NetworkAccess::Request(const QUrl & url, const RawHeaderPairs & raw_header)
{
    return SendRequest(url, raw_header, "GET", QByteArray());
}

NetworkReply NetworkAccess::Put(const QUrl & url, const RawHeaderPairs & raw_header, const QByteArray & data)
{
    return SendRequest(url, raw_header, "PUT", data);
}

NetworkReply NetworkAccess::Post(const QUrl & url, const RawHeaderPairs & raw_header, const QByteArray & data)
{
    return SendRequest(url, raw_header, "POST", data);
}

NetworkReply NetworkAccess::Delete(const QUrl & url, const RawHeaderPairs & raw_header, const QByteArray & data)
{
    return SendRequest(url, raw_header, "DELETE", data);
}

void NetworkAccess::HandleReply(QNetworkReply * reply, NetworkReply & m_reply)
{
    m_reply.error = reply->error();
    m_reply.data = reply->readAll();

    if (m_reply.error != QNetworkReply::NoError) {
        qWarning() << "request reply error" << m_reply.error << reply->request().url() << m_reply.data;
    }

    reply->deleteLater();
}

QNetworkRequest NetworkAccess::NetworkRequest(const QUrl & url)
{
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    return request;
}

NetworkReply NetworkAccess::SendRequest(
  const QUrl & url,
  const RawHeaderPairs & header,
  const QByteArray & action,
  const QByteArray & data)
{
    QEventLoop loop;

    NetworkReply reply;
    if (!url.isValid()) {
        return reply;
    }
    QNetworkAccessManager net_access_mgr;
    connect(&net_access_mgr, &QNetworkAccessManager::finished,
            [=, &reply, &loop](QNetworkReply * p_reply) {
                HandleReply(p_reply, reply);
                loop.quit();
            });
    auto request(NetworkRequest(url));
    for (const auto & header : header) {
        request.setRawHeader(header.first, header.second);
    }
    net_access_mgr.sendCustomRequest(request, action, data);

    loop.exec();

    return reply;
}
