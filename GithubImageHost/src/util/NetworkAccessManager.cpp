// NetworkAccessManager.cpp
#include "NetworkAccessManager.h"

NetworkAccessManager::NetworkAccessManager(QObject* parent) : QObject(parent), m_IsSyncCallInProgress(false)
{
	m_NetworkManager = new QNetworkAccessManager(this);

	m_TimeoutTimer = new QTimer(this);
	connect(m_TimeoutTimer, &QTimer::timeout, this, &NetworkAccessManager::OnTimeout);
}

NetworkAccessManager::~NetworkAccessManager()
{
	delete m_NetworkManager;
}

void NetworkAccessManager::SendRequest(
	const QUrl& url, 
	const RawHeaderPairs& headerPairs,
	RequestMethod method, 
	const QByteArray& data)
{
	if (m_IsSyncCallInProgress)
	{
		qWarning() << "Async call in progress. Ignoring the request.";
		return;
	}

	connect(m_NetworkManager, &QNetworkAccessManager::finished, 
		this, &NetworkAccessManager::OnReplyFinished, Qt::UniqueConnection);


	QNetworkRequest request(url);
	for (const auto& header : headerPairs) {
		request.setRawHeader(header.first, header.second);
	}

	switch (method)
	{
	case Get:
		m_NetworkManager->get(request);
		break;
	case Post:
		m_NetworkManager->post(request, data);
		break;
	case Put:
		m_NetworkManager->put(request, data);
		break;
	case Delete:
		m_NetworkManager->deleteResource(request);
		break;
	}
}

QByteArray NetworkAccessManager::SendRequestSync(
	const QUrl& url, 
	const RawHeaderPairs& headerPairs,
	RequestMethod method, 
	const QByteArray& data, 
	int timeout)
{
	if (m_IsSyncCallInProgress)
	{
		qWarning() << "Sync call in progress. Ignoring the request.";
		return QByteArray();
	}

	disconnect(m_NetworkManager, &QNetworkAccessManager::finished,
		this, &NetworkAccessManager::OnReplyFinished);

	m_IsSyncCallInProgress = true;

	QNetworkRequest request(url);
	for (const auto& header : headerPairs) {
		request.setRawHeader(header.first, header.second);
	}
	QNetworkReply* reply = nullptr;

	switch (method)
	{
	case Get:
		reply = m_NetworkManager->get(request);
		break;
	case Post:
		reply = m_NetworkManager->post(request, data);
		break;
	case Put:
		reply = m_NetworkManager->put(request, data);
		break;
	case Delete:
		reply = m_NetworkManager->deleteResource(request);
		break;
	}

	m_TimeoutTimer->start(timeout);

	QEventLoop loop;
	connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	connect(m_TimeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
	loop.exec(QEventLoop::ExcludeUserInputEvents);

	m_TimeoutTimer->stop();
	m_IsSyncCallInProgress = false;

	m_LastStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	QByteArray responseData = reply->readAll();
	
	reply->deleteLater();
	return responseData;
}

int NetworkAccessManager::GetLastStatusCode() const
{
	return m_LastStatusCode;
}

void NetworkAccessManager::OnReplyFinished(QNetworkReply* reply)
{
	m_TimeoutTimer->stop();

	m_LastStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	if (reply->error() == QNetworkReply::NoError)
	{
		qInfo() << reply->readAll();
		emit SgnRequestFinished(reply->readAll());
	}
	else
	{
		qInfo() << reply->errorString();
		emit SgnRequestError(reply->errorString());
	}

	reply->deleteLater();
}

void NetworkAccessManager::OnTimeout()
{
	m_TimeoutTimer->stop();
	emit SgnRequestError("Request timeout");

	m_IsSyncCallInProgress = false;
}
