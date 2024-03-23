// NetworkAccessManager.h
#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QTimer>

class NetworkAccessManager : public QObject
{
	Q_OBJECT
public:
	typedef QVector<QPair<QByteArray, QByteArray>> RawHeaderPairs;
	enum RequestMethod
	{
		Get,
		Post,
		Put,
		Delete
	};

public:
	explicit NetworkAccessManager(QObject* parent = nullptr);
	~NetworkAccessManager();

	void SendRequest(
		const QUrl& url, 
		const RawHeaderPairs& header,
		RequestMethod method, 
		const QByteArray& data = QByteArray());

	QByteArray SendRequestSync(
		const QUrl& url, 
		const RawHeaderPairs& header,
		RequestMethod method, 
		const QByteArray& data = QByteArray(), 
		int timeout = 30000);

	int GetLastStatusCode() const;

signals:
	void SgnRequestFinished(const QByteArray& responseData);
	void SgnRequestError(const QString& errorString);

private slots:
	void OnReplyFinished(QNetworkReply* reply);
	void OnTimeout();

private:
	QNetworkAccessManager* m_NetworkManager;
	QTimer* m_TimeoutTimer;
	int m_LastStatusCode{ 0 };
	bool m_IsSyncCallInProgress;
};

#endif // NETWORKACCESSMANAGER_H

