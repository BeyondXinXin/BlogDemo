#include "qapplication.h"
#include "qfile.h"
#include "qtextcodec.h"
#include "util/appinit.h"
#include "window/main_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setFont(QFont("Microsoft Yahei", 9));

    QUIHelper::Instance()->start();
    QUIHelper::SetStyle();

    MainWindow w;
    w.show();

    return a.exec();
}

#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QIODevice>
#include <QImageReader>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QMovie>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QPair>
#include <QPixmap>
#include <QSsl>
#include <QThread>
#include <QUrl>
#include <QVector>
#include <QtConcurrent>

//struct GHImage
//{
//    QString html_url { "" };
//    QString download_url { "" };
//    QString name { "" };
//    QString github_path { "" };
//    QString sha { "" };
//    QImage pix;
//};
//static QMap<QString, QList<GHImage>> git_images;

//struct NetworkReply
//{
//    QNetworkReply::NetworkError error = QNetworkReply::HostNotFoundError;
//    QByteArray data;
//};

//void SleepWait(const int &seconds)
//{
//    if (seconds <= 0) {
//        return;
//    }
//    QElapsedTimer t;
//    t.start();
//    while (t.elapsed() < seconds) {
//        QCoreApplication::processEvents();
//    }
//}

//QNetworkRequest NewRequest(const QUrl &url)
//{
//    QNetworkRequest request(url);
//    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//    request.setRawHeader(QByteArray("Authorization"), token.toUtf8());
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github.v3+json"));
//    return request;
//}

//NetworkReply HandleReply(QNetworkReply *t_reply)
//{
//    NetworkReply reply;
//    reply.error = t_reply->error();
//    reply.data = t_reply->readAll();
//    return reply;
//}

//QStringList ParsingJSON(NetworkReply net_r, const QString &dir)
//{

//    QStringList dirs;

//    auto json_doc = QJsonDocument::fromJson(net_r.data);

//    if (json_doc.isArray()) {
//        auto json_array = json_doc.array();

//        QJsonArray::Iterator i = json_array.begin();
//        while (i != json_array.end()) {
//            if (i->isObject()) {
//                auto tmp_json_obj = i->toObject();
//                const auto type = tmp_json_obj[QStringLiteral("type")].toString();
//                const auto path = tmp_json_obj[QStringLiteral("path")].toString();
//                if ("dir" == type) {
//                    dirs << path;
//                } else if ("file" == type) {
//                    const auto name = tmp_json_obj[QStringLiteral("name")].toString();
//                    if (name.right(6).contains("bmp")
//                        || name.right(6).contains("gif")
//                        || name.right(6).contains("jpg")
//                        || name.right(6).contains("jpeg")
//                        || name.right(6).contains("png")) {
//                        GHImage image;
//                        image.html_url = tmp_json_obj[QStringLiteral("html_url")].toString();
//                        image.download_url = tmp_json_obj[QStringLiteral("download_url")].toString();
//                        image.name = name;
//                        image.github_path = tmp_json_obj[QStringLiteral("path")].toString();
//                        image.sha = tmp_json_obj[QStringLiteral("sha")].toString();
//                        git_images[dir] << image;
//                    }
//                }
//            }
//            i++;
//        }
//    }

//    return dirs;
//}

//QStringList QueryImage(const QString &path)
//{
//    const auto url = QUrl(QString("%1/repos/%2/%3/contents/%4").arg(c_apiUrl, userName, repoName, path));
//    QEventLoop loop;
//    auto net_q = NewRequest(url);
//    QNetworkAccessManager net_access;
//    NetworkReply net_r;
//    QObject::connect(
//      &net_access, &QNetworkAccessManager::finished, [&net_r, &loop](QNetworkReply *p_reply) {
//          net_r = HandleReply(p_reply);
//          loop.quit();
//      });
//    net_access.sendCustomRequest(net_q, "GET", QByteArray());
//    loop.exec();
//    auto recursive_path = ParsingJSON(net_r, path);
//    return recursive_path;
//}

//void DownloadImage(GHImage &gh_image)
//{
//    if (gh_image.pix.isNull()) {
//        QEventLoop loop;
//        QNetworkAccessManager net_access;
//        NetworkReply net_r;
//        auto net_q = NewRequest(QUrl(gh_image.download_url));
//        QObject::connect(&net_access, &QNetworkAccessManager::finished, [&net_r, &loop](QNetworkReply *p_reply) {
//            net_r = HandleReply(p_reply);
//            loop.quit();
//        });
//        net_access.sendCustomRequest(net_q, "GET", QByteArray());
//        loop.exec();

//        if (!gh_image.html_url.right(5).contains("gif")) {
//            gh_image.pix = QImage::fromData(net_r.data);
//        } else {
//            QBuffer buffer;
//            buffer.open(QFile::WriteOnly);
//            buffer.write(net_r.data);
//            buffer.close();
//            QImageReader reader(&buffer);
//            gh_image.pix = reader.read();
//        }
//    }
//}

//void AppengQureData(QStringList &list, const QStringList &data)
//{
//    list << data;
//}

const QString c_apiUrl = "https://api.github.com";
//const QString token = "token ghp_xxx";
//const QString userName = "BeyondXinXin";
//const QString repoName = "BeyondXinXIn";

int mai11n(int argc, char *argv[])
{
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss:zzz");

    QApplication a(argc, argv);

    //        QStringList list { "" };
    //        while (!list.isEmpty()) {
    //            QFuture<QStringList> f = QtConcurrent::mappedReduced(list, &QueryImage, &AppengQureData);
    //            f.waitForFinished();
    //            list.clear();
    //            list << f.result();
    //        }

    //    foreach (auto key, git_images.keys()) {
    //        qDebug() << "------------------------------------------------";
    //        qDebug() << "path:" << key << "size:" << git_images[key].size();
    //        QtConcurrent::blockingMap(git_images[key], &DownloadImage);
    //    }

    QString p_path = "PixX/test.png";

    QEventLoop loop;
    QString token, userName, repoName;
    token = "ghp_xxx";
    userName = "BeyondXinXin";
    repoName = "BeyondXinXIn";
    // const auto urlStr = QUrl(QString("%1/repos/%2/%3").arg(c_apiUrl, userName, repoName));

    //    QNetworkAccessManager netAccessMgr;
    //    qDebug() << netAccessMgr.supportedSchemes();
    //    QObject::connect(&netAccessMgr, &QNetworkAccessManager::finished,
    //                     [&loop](QNetworkReply *p_reply) {
    //                         qDebug() << "++++++++++++++++++netAccessMgr finished++++++++++++++++++";
    //                         qDebug() << p_reply->error();
    //                         qDebug() << p_reply->readAll();
    //                         loop.quit();
    //                     });
    //    QNetworkRequest request(urlStr);
    //    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    //    request.setRawHeader(QByteArray("Authorization"), token.toUtf8());
    //    netAccessMgr.sendCustomRequest(request, "GET", QByteArray());

    const auto urlStr = QUrl(QString("%1/repos/%2/%3/contents/%4").arg(c_apiUrl, userName, repoName, p_path));

    QNetworkAccessManager netAccessMgr;
    qDebug() << urlStr;
    QObject::connect(&netAccessMgr, &QNetworkAccessManager::finished,
                     [&loop](QNetworkReply *p_reply) {
                         qDebug() << "++++++++++++++++++netAccessMgr finished++++++++++++++++++";
                         qDebug() << p_reply->error();
                         QByteArray data = p_reply->readAll();
                         qDebug() << data;
                         loop.quit();
                     });

    QJsonObject requestDataObj;
    requestDataObj[QStringLiteral("message")] = QString("remove data");
    requestDataObj[QStringLiteral("sha")] = "a3cface003d5da5b8c399d07d95c7f1bb4ee1507";
    QJsonDocument doc(requestDataObj);

    QNetworkRequest request(urlStr);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader(QByteArray("Authorization"), token.toUtf8());
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github.v3+json"));

    netAccessMgr.sendCustomRequest(request, "DELETE", doc.toJson(QJsonDocument::Compact));

    loop.exec();

    return 0 /*QApplication::exec()*/;
}

// Create a blob
//QString token, userName, repoName, p_path;
//token = "token ghp_xxx";
//userName = "BeyondXinXin";
//repoName = "BeyondXinXIn";
//p_path = "PixX/test.png";
//p_path = "PixX/test.png";
//const auto urlStr = QUrl(QString("%1/repos/%2/%3/git/blobs").arg(c_apiUrl, userName, repoName));

//QNetworkAccessManager netAccessMgr;
//qDebug() << netAccessMgr.supportedSchemes();
//QObject::connect(&netAccessMgr, &QNetworkAccessManager::finished,
//                 [&finished](QNetworkReply *p_reply) {
//                     qDebug() << "++++++++++++++++++netAccessMgr finished++++++++++++++++++";
//                     qDebug() << p_reply->error();
//                     QByteArray data = p_reply->readAll();
//                     qDebug() << data;
//                     QJsonObject requestDataObj = QJsonDocument::fromJson(data).object();
//                     const auto sha = requestDataObj[QStringLiteral("sha")].toString();
//                     qDebug() << "sha:" << sha;
//                     finished = true;
//                 });

//QJsonObject requestDataObj;
//requestDataObj[QStringLiteral("content")] = QString("content");
//QJsonDocument doc(requestDataObj);

//QNetworkRequest request(urlStr);
//request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//request.setRawHeader(QByteArray("Authorization"), token.toUtf8());
//request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github.v3+json"));
//netAccessMgr.sendCustomRequest(request, "POST", doc.toJson(QJsonDocument::Compact));

// testConfig

//        QString token, userName, repoName;
//        token = "token ghp_xxx";
//        userName = "BeyondXinXin";
//        repoName = "BeyondXinXIn";
//        const auto urlStr = QUrl(QString("%1/repos/%2/%3").arg(c_apiUrl, userName, repoName));

//        QNetworkAccessManager netAccessMgr;
//        qDebug() << netAccessMgr.supportedSchemes();
//        QObject::connect(&netAccessMgr, &QNetworkAccessManager::finished,
//                         [&finished](QNetworkReply *p_reply) {
//                             qDebug() << "++++++++++++++++++netAccessMgr finished++++++++++++++++++";
//                             qDebug() << p_reply->error();
//                             qDebug() << p_reply->readAll();
//                             finished = true;
//                         });
//        QNetworkRequest request(urlStr);
//        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//        request.setRawHeader(QByteArray("Authorization"), token.toUtf8());
//        netAccessMgr.sendCustomRequest(request, "GET", QByteArray());

// query

//    QString token, userName, repoName, p_path;
//    token = "token ghp_xxx";
//    userName = "BeyondXinXin";
//    repoName = "BeyondXinXIn";
//    p_path = "PixX/test.png";
//    p_path = "PixX/test.png";
//    const auto urlStr = QUrl(QString("%1/repos/%2/%3/contents/%4").arg(c_apiUrl, userName, repoName, p_path));

//    QNetworkAccessManager netAccessMgr;
//    qDebug() << netAccessMgr.supportedSchemes();
//    QObject::connect(&netAccessMgr, &QNetworkAccessManager::finished,
//                     [&finished](QNetworkReply *p_reply) {
//                         qDebug() << "++++++++++++++++++netAccessMgr finished++++++++++++++++++";
//                         qDebug() << p_reply->error();
//                         QByteArray data = p_reply->readAll();
//                         qDebug() << data;
//                         QJsonObject requestDataObj = QJsonDocument::fromJson(data).object();
//                         const auto sha = requestDataObj[QStringLiteral("sha")].toString();
//                         qDebug() << "sha:" << sha;
//                         finished = true;
//                     });
//    QNetworkRequest request(urlStr);
//    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//    request.setRawHeader(QByteArray("Authorization"), token.toUtf8());
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github.v3+json"));
//    netAccessMgr.sendCustomRequest(request, "GET", QByteArray());

// create

//    QString token, userName, repoName, p_path;
//    token = "token ghp_xxx";
//    userName = "BeyondXinXin";
//    repoName = "BeyondXinXIn";
//    p_path = "PixX/test.png";
//    const auto urlStr = QUrl(QString("%1/repos/%2/%3/contents/%4").arg(c_apiUrl, userName, repoName, p_path));

//    QNetworkAccessManager netAccessMgr;
//    qDebug() << netAccessMgr.supportedSchemes();
//    QObject::connect(&netAccessMgr, &QNetworkAccessManager::finished,
//                     [&finished](QNetworkReply *p_reply) {
//                         qDebug() << "++++++++++++++++++netAccessMgr finished++++++++++++++++++";
//                         qDebug() << p_reply->error();
//                         qDebug() << p_reply->readAll();
//                         finished = true;
//                     });

//    // 图片转data
//    QBuffer buffer;
//    buffer.open(QIODevice::ReadWrite);
//    QPixmap pixmap("01.jpg");
//    pixmap.save(&buffer, "jpg");
//    QByteArray p_data;
//    p_data.append(buffer.data());

//    QJsonObject requestDataObj;
//    requestDataObj[QStringLiteral("message")] = QString("VX_ADD: %1").arg(p_path);
//    requestDataObj[QStringLiteral("content")] = QString::fromUtf8(p_data.toBase64());
//    QJsonDocument doc(requestDataObj);

//    QNetworkRequest request(urlStr);
//    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//    request.setRawHeader(QByteArray("Authorization"), token.toUtf8());
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github.v3+json"));
//    netAccessMgr.sendCustomRequest(request, "PUT", doc.toJson(QJsonDocument::Compact));

// remove

//    QString token, userName, repoName, p_path, sha, m_imageUrlPrefix, resourcePath, p_url;
//    token = "token ghp_xxx";
//    userName = "BeyondXinXin";
//    repoName = "BeyondXinXIn";
//    p_path = "PixX/test.png";
//    sha = "a3cface003d5da5b8c399d07d95c7f1bb4ee1507";
//    p_url = "PixX/test.png";

//    m_imageUrlPrefix = QString("https://raw.githubusercontent.com/%1/%2/main/").arg(userName, repoName);
//    resourcePath = p_url.mid(m_imageUrlPrefix.size());

//    const auto urlStr = QUrl(QString("%1/repos/%2/%3/contents/%4").arg(c_apiUrl, userName, repoName, p_path));

//    QNetworkAccessManager netAccessMgr;
//    qDebug() << netAccessMgr.supportedSchemes();
//    QObject::connect(&netAccessMgr, &QNetworkAccessManager::finished,
//                     [&finished](QNetworkReply *p_reply) {
//                         qDebug() << "++++++++++++++++++netAccessMgr finished++++++++++++++++++";
//                         qDebug() << p_reply->error();
//                         QByteArray data = p_reply->readAll();
//                         qDebug() << data;
//                         finished = true;
//                     });

//    QJsonObject requestDataObj;
//    requestDataObj[QStringLiteral("message")] = QString("remove data");
//    requestDataObj[QStringLiteral("sha")] = sha;
//    QJsonDocument doc(requestDataObj);

//    QNetworkRequest request(urlStr);
//    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//    request.setRawHeader(QByteArray("Authorization"), token.toUtf8());
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github.v3+json"));

//    netAccessMgr.sendCustomRequest(request, "DELETE", doc.toJson(QJsonDocument::Compact));
