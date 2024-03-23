// GitHubImageHost.h
#ifndef GITHUBIMAGEHOST_H
#define GITHUBIMAGEHOST_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QImage>

#include "NetworkAccessManager.h"

struct GHInfo
{
	QString ghToken;
	QString userName;
	QString repoName;
};

struct GHImage
{
	QString htmlUrl{ "" };
	QString downloadUrl{ "" };
	QString name{ "" };
	QString githubDirectory{ "" };
	QString githubPath{ "" };
	QString sha{ "" };
	QImage pix;

	// 添加序列化和反序列化操作符
	friend QDataStream& operator<<(QDataStream& out, const GHImage& image)
	{
		out << image.htmlUrl << image.downloadUrl << image.name
			<< image.githubDirectory << image.githubPath << image.sha << image.pix;
		return out;
	}

	friend QDataStream& operator>>(QDataStream& in, GHImage& image)
	{
		in >> image.htmlUrl >> image.downloadUrl >> image.name
			>> image.githubDirectory >> image.githubPath >> image.sha >> image.pix;
		return in;
	}
};

using GHImageMap = QMap<QString, QList<GHImage>>;

class GitHubImageHost : public QObject
{
	Q_OBJECT
public:
	explicit GitHubImageHost(QObject* parent = nullptr);

	void SaveGHInfoConfig();
	void LoadGHInfoConfig();

	void SaveGHImageMapCache();
	void LoadGHImageMapCache();

	bool TestConfig(GHInfo& ghInfo, QString& errStr);

	bool GetAllImage();
	bool AddImage(const QString& filePath);
	bool RemoveImage(const GHImage& githubPath);

	GHInfo GetGHInfo() const;
	void SetGHInfo(const GHInfo& ghInfo);

private:
	NetworkAccessManager::RawHeaderPairs PrepareCommonHeaders(const QString& token);
	QJsonArray GetRepoContents(const QString& path = "");

private:
	GHInfo m_GHInfo;
	NetworkAccessManager::RawHeaderPairs m_commonHeaders;
	NetworkAccessManager m_NetworkManager;
	QStringList m_ImageTypes;

public:
	static GHImageMap m_GHImageMap;
	static const QSize m_ImageSize;

private:
	static const QString GHApiUrl;
	static const QString GHInfoConfigPath;
	static const QString GHImageMapCachePath;
};

#endif // GITHUBIMAGEHOST_H
