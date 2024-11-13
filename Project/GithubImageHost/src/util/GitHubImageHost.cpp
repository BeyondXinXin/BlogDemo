#include "GitHubImageHost.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QFileInfo>

GHImageMap GitHubImageHost::m_GHImageMap;
const QSize GitHubImageHost::m_ImageSize(200, 150);

const QString GitHubImageHost::GHApiUrl = "https://api.github.com";
const QString GitHubImageHost::GHInfoConfigPath = "github_info.cfg";
const QString GitHubImageHost::GHImageMapCachePath = "github_imgmap_cache.cfg";


namespace {
	QByteArray serializeGHImageList(const QList<GHImage>& imageList)
	{
		QByteArray byteArray;
		QDataStream stream(&byteArray, QIODevice::WriteOnly);
		stream << imageList;
		return byteArray;
	}

	QList<GHImage> deserializeGHImageList(const QByteArray& byteArray)
	{
		QList<GHImage> imageList;
		QDataStream stream(byteArray);
		stream >> imageList;
		return imageList;
	}
}

GitHubImageHost::GitHubImageHost(QObject* parent) : QObject(parent)
{
	connect(&m_NetworkManager, &NetworkAccessManager::SgnRequestFinished, this,
		[this](const QByteArray& responseData) {

		});

	connect(&m_NetworkManager, &NetworkAccessManager::SgnRequestError,
		this, [this](const QString& errorString) {

		});

	m_ImageTypes = QStringList{ "bmp" ,"gif" , "jpg" , "jpeg" , "png" , "webp" };

	LoadGHInfoConfig();
	QString errorString;
	if (TestConfig(m_GHInfo, errorString))
	{
		m_commonHeaders = PrepareCommonHeaders(m_GHInfo.ghToken);
	}
	else
	{
		m_GHInfo.ghToken = "";
		m_GHInfo.userName = "";
		m_GHInfo.repoName = "";
	}

	LoadGHImageMapCache();
}

void GitHubImageHost::SaveGHInfoConfig()
{
	QFile file(GHInfoConfigPath);
	if (file.open(QIODevice::WriteOnly))
	{
		QDataStream out(&file);
		out << m_GHInfo.ghToken << m_GHInfo.userName << m_GHInfo.repoName;
		file.close();
	}
}

void GitHubImageHost::LoadGHInfoConfig()
{
	QFile file(GHInfoConfigPath);
	if (file.open(QIODevice::ReadOnly))
	{
		QDataStream in(&file);
		in >> m_GHInfo.ghToken >> m_GHInfo.userName >> m_GHInfo.repoName;
		file.close();
	}
}

bool GitHubImageHost::TestConfig(GHInfo& ghInfo, QString& errStr)
{
	QUrl userInfoUrl = QUrl(QString("%1/user").arg(GHApiUrl));
	QNetworkRequest userInfoRequest(userInfoUrl);
	QByteArray userInforesponseData = m_NetworkManager.SendRequestSync(
		userInfoUrl, PrepareCommonHeaders(ghInfo.ghToken), NetworkAccessManager::Get);
	if (200 == m_NetworkManager.GetLastStatusCode())
	{
		QJsonDocument jsonDoc = QJsonDocument::fromJson(userInforesponseData);
		if (jsonDoc.isObject()) {
			QJsonObject jsonObject = jsonDoc.object();
			if (jsonObject.contains("login")) {
				ghInfo.userName = jsonObject["login"].toString();
			}
		}
	}
	else {
		errStr = QString(u8"其他错误:").arg(QString::number(m_NetworkManager.GetLastStatusCode()));
		return false;
	}

	qInfo() << ghInfo.userName;
	
	QUrl testUrl(QString("%1/repos/%2/%3").arg(GHApiUrl).arg(ghInfo.userName).arg(ghInfo.repoName));
	QNetworkRequest request(testUrl);
	QByteArray responseData = m_NetworkManager.SendRequestSync(
		testUrl, PrepareCommonHeaders(m_GHInfo.ghToken), NetworkAccessManager::Get);

	switch (m_NetworkManager.GetLastStatusCode())
	{
	case 200:
		// 验证成功
		return true;
	case 404:
		errStr = u8"仓库不存在";
		break;
	case 401:
		errStr = u8"未授权，可能是 token 无效";
		break;
	default:
		errStr = QString(u8"其他错误:").arg(QString::number(m_NetworkManager.GetLastStatusCode()));
		break;
	}
	return false;
}

bool GitHubImageHost::GetAllImage()
{
	QStringList secondaryDirectorys;
	QJsonArray jsonArray = GetRepoContents();
	for (const QJsonValue& value : jsonArray)
	{
		QJsonObject jsonObject = value.toObject();
		if (jsonObject["type"].toString() == "dir")
		{
			secondaryDirectorys.append(jsonObject["name"].toString());
		}
	}
	if (secondaryDirectorys.isEmpty())
	{
		return false;
	}
	m_GHImageMap.clear();
	for (const QString& secondaryDirectory : secondaryDirectorys)
	{
		QJsonArray jsonArray = GetRepoContents(secondaryDirectory);
		for (const QJsonValue& value : jsonArray)
		{
			QJsonObject jsonObject = value.toObject();
			QFileInfo fileInfo(jsonObject["name"].toString());
			QString fileExtension = fileInfo.suffix();
			if (jsonObject["type"].toString() != "file" ||!m_ImageTypes.contains(fileExtension))
			{
				break;
			}
			GHImage image;
			image.name = jsonObject["name"].toString();
			image.htmlUrl = jsonObject["html_url"].toString();
			image.downloadUrl = jsonObject["download_url"].toString();
			image.githubDirectory = "ImageHost";
			image.githubPath = image.githubDirectory + "/" + image.name;
			image.sha = jsonObject["sha"].toString();
			QByteArray imageData = m_NetworkManager.SendRequestSync(
				QUrl(image.downloadUrl), m_commonHeaders, NetworkAccessManager::Get);
			QImage originalImage;
			originalImage.loadFromData(imageData);
			image.pix = originalImage.scaled(
				200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			m_GHImageMap[secondaryDirectory].append(image);
		}
	}
	return true;
}


bool GitHubImageHost::AddImage(const QString& filePath)
{
	QString directory = "ImageHost/" + QDate::currentDate().toString("yyyyMM");
	QString fileName = QString::number(QRandomGenerator::global()->generate(), 36).left(7) + ".png";
	QString uploadUrl = QString("%1/repos/%2/%3/contents/%4/%5")
		.arg(GHApiUrl)
		.arg(m_GHInfo.userName)
		.arg(m_GHInfo.repoName)
		.arg(directory)
		.arg(fileName);

	// 设置请求头，使用给定的 token 进行验证
	QNetworkRequest request(uploadUrl);

	// 构建请求数据
	QJsonObject requestData;
	requestData["message"] = "Add image";
	requestData["committer"] = QJsonObject{ {"name", m_GHInfo.userName}, {"email", "example@example.com"} };

	// 读取图片文件的二进制数据
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		// 处理文件打开失败的情况
		return false;
	}
	QByteArray fileData = file.readAll();
	file.close();
	requestData["content"] = QString::fromUtf8(fileData.toBase64());
	requestData["path"] = directory + "/" + fileName;

	// 发送 PUT 请求，上传图片文件，使用同步接口
	QByteArray requestDataJson = QJsonDocument(requestData).toJson(QJsonDocument::Compact);
	QByteArray responseData = m_NetworkManager.SendRequestSync(
		uploadUrl, m_commonHeaders, NetworkAccessManager::Put, requestDataJson);

	if (m_NetworkManager.GetLastStatusCode() == 201)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool GitHubImageHost::RemoveImage(const GHImage& ghImage)
{
	// 构建删除图片的 URL
	QString deleteUrl = QString("%1/repos/%2/%3/contents/%4")
		.arg(GHApiUrl)
		.arg(m_GHInfo.userName)
		.arg(m_GHInfo.repoName)
		.arg(ghImage.githubPath);

	// 设置请求头，使用给定的 token 进行验证
	QNetworkRequest request(deleteUrl);

	// 构建请求数据
	QJsonObject requestData;
	requestData["message"] = "Remove image";
	requestData["committer"] = QJsonObject{ {"name", m_GHInfo.userName}, {"email", "example@example.com"} };
	requestData["sha"] = ghImage.sha;

	// 发送 DELETE 请求，删除图片文件，使用同步接口
	QByteArray requestDataJson = QJsonDocument(requestData).toJson(QJsonDocument::Compact);
	QByteArray responseData = m_NetworkManager.SendRequestSync(
		deleteUrl, m_commonHeaders, NetworkAccessManager::Delete, requestDataJson);

	if (m_NetworkManager.GetLastStatusCode() == 200)
	{
		return true;
	}
	else
	{
		return false;
	}
}

GHInfo GitHubImageHost::GetGHInfo() const
{
	return m_GHInfo;
}

void GitHubImageHost::SetGHInfo(const GHInfo& ghInfo)
{
	m_GHInfo = ghInfo;
	m_commonHeaders = PrepareCommonHeaders(m_GHInfo.ghToken);
	SaveGHInfoConfig();
}

void GitHubImageHost::SaveGHImageMapCache()
{
	QFile file(GHImageMapCachePath);
	if (file.open(QIODevice::WriteOnly))
	{
		QDataStream out(&file);
		for (auto it = m_GHImageMap.constBegin(); it != m_GHImageMap.constEnd(); ++it)
		{
			out << it.key();
			out << serializeGHImageList(it.value());
		}
		file.close();
	}
}

void GitHubImageHost::LoadGHImageMapCache()
{
	QFile file(GHImageMapCachePath);
	if (file.open(QIODevice::ReadOnly))
	{
		QDataStream in(&file);
		m_GHImageMap.clear();
		while (!in.atEnd())
		{
			QString key;
			QByteArray valueBytes;
			in >> key;
			in >> valueBytes;
			QList<GHImage> value = deserializeGHImageList(valueBytes);

			m_GHImageMap.insert(key, value);
		}

		file.close();
	}
}

NetworkAccessManager::RawHeaderPairs GitHubImageHost::PrepareCommonHeaders(const QString& token)
{
	using pair = QPair<QByteArray, QByteArray>;
	NetworkAccessManager::RawHeaderPairs header;
	pair authorization(QByteArray("Authorization"), "token " + token.toUtf8());
	pair accept(QByteArray("Accept"), QByteArray("application/vnd.github.v3+json"));
	header.push_back(authorization);
	header.push_back(accept);
	return header;
}

QJsonArray GitHubImageHost::GetRepoContents(const QString &path)
{
	QString imageUrl = QString("%1/repos/%2/%3/contents/ImageHost/%4")
		.arg(GHApiUrl).arg(m_GHInfo.userName).arg(m_GHInfo.repoName).arg(path);
	if (path.isEmpty()) {
		imageUrl = QString("%1/repos/%2/%3/contents/ImageHost")
			.arg(GHApiUrl).arg(m_GHInfo.userName).arg(m_GHInfo.repoName);
	}
	QNetworkRequest request(imageUrl);
	QByteArray responseData = m_NetworkManager.SendRequestSync(
		QUrl(imageUrl), m_commonHeaders, NetworkAccessManager::RequestMethod::Get);
	QJsonArray jsonArray;
	if (m_NetworkManager.GetLastStatusCode() == 200)
	{
		QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
		if (jsonDoc.isArray())
		{
			jsonArray = jsonDoc.array();
		}
	}
	return jsonArray;
}
