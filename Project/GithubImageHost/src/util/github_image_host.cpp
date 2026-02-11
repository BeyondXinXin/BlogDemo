#include "github_image_host.h"

#include <QImageReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtConcurrent>

const QString GitHubImageHost::gh_api_url_ = "https://api.github.com";
const QString GitHubImageHost::gothubinfo_cfg_ = "githubinfo.cfg";
const QString GitHubImageHost::image_map_cfg_ = "githubimgmap.cfg";

GithubInfo GitHubImageHost::info_;

QSize GitHubImageHost::img_size_(200, 150);

GitHubImageHost::GHImageMap GitHubImageHost::images_;

void FunAppedQureData(QStringList &list, const QStringList &data)
{
    list << data;
};

void SleepWait(const int &seconds)
{
    if (seconds <= 0) {
        return;
    }
    QElapsedTimer t;
    t.start();
    while (t.elapsed() < seconds) {
        QCoreApplication::processEvents();
    }
}

GitHubImageHost::GitHubImageHost(QObject *parent)
  : QObject(parent)
{
}

bool GitHubImageHost::GetAllImage(QString &msg)
{

    if (info_.isEmpty()) {
        msg = u8"图床未设置参数";
        return false;
    }

    images_.clear();
    QStringList list { "" };
    while (!list.isEmpty()) {
        QFuture<QStringList> f = QtConcurrent::mappedReduced(list, &GitHubImageHost::QueryImage, &FunAppedQureData);
        bool finished = false;
        while (!finished) {
            SleepWait(100);
            if (f.isRunning()) {
                finished = true;
            }
        }
        list.clear();
        list << f.result();
    }

    foreach (auto key, images_.keys()) {
        qDebug() << "------------------------------------------------";
        qDebug() << "path:" << key << "size:" << images_[key].size();
        QtConcurrent::blockingMap(images_[key], &DownloadImage);
    }

    WriteImages();

    return true;
}

bool GitHubImageHost::TestConfig(const GithubInfo &jobj, QString &msg)
{
    QString token, user_name, repo_name;
    ParseConfig(jobj, token, user_name, repo_name);

    if (token.isEmpty() || user_name.isEmpty() || repo_name.isEmpty()) {
        msg = u8"参数为空";
        return false;
    }

    auto header = PrepareCommonHeaders(token);
    const auto url_str = QString("%1/repos/%2/%3").arg(gh_api_url_, user_name, repo_name);
    auto reply = NetworkAccess::Request(QUrl(url_str), header);

    msg = QString::fromUtf8(reply.data);
    return reply.error == QNetworkReply::NoError;
}

QString GitHubImageHost::Create(
  const QByteArray &data,
  const QString &path,
  QString &msg)
{
    QString dest_url;

    if (path.isEmpty()) {
        msg = u8"路径空";
        return dest_url;
    }

    if (info_.isEmpty()) {
        msg = u8"图床未设置参数";
        return dest_url;
    }

    auto header = PrepareCommonHeaders(info_.gh_token);
    const auto url_str = QString("%1/repos/%2/%3/contents/%4")
                           .arg(gh_api_url_, info_.user_name, info_.repo_name, path);

    auto reply = NetworkAccess::Request(QUrl(url_str), header);
    if (reply.error == QNetworkReply::NoError) {
        msg = u8"文件已经存在";
        return dest_url;
    } else if (reply.error != QNetworkReply::ContentNotFoundError) {
        msg = QString(u8"无法添加,%1)").arg(QString(reply.data));
        return dest_url;
    }

    QJsonObject request_data_obj;
    request_data_obj[QStringLiteral("message")] = QString("ADD: %1").arg(path);
    request_data_obj[QStringLiteral("content")] = QString::fromUtf8(data.toBase64());
    QJsonDocument doc(request_data_obj);

    reply = NetworkAccess::Put(QUrl(url_str), header, doc.toJson(QJsonDocument::Compact));

    if (reply.error != QNetworkReply::NoError) {
        msg = QString(u8"无法添加,%1)").arg(QString(reply.data));
        return dest_url;
    } else {
        QJsonObject reply_obj = QJsonDocument::fromJson(reply.data).object();
        auto target_url =
          reply_obj[QStringLiteral("content")].toObject().value(QStringLiteral("download_url")).toString();
        if (target_url.isEmpty()) {
            msg = QString(u8"无法添加,%1)").arg(QString(reply.data));
        }
        return target_url;
    }
}

bool GitHubImageHost::Remove(const QString &path, const QString &github_path, const QString &sha, QString &msg)
{

    const auto url_str = QString("%1/repos/%2/%3/contents/%4")
                           .arg(gh_api_url_, info_.user_name, info_.repo_name, github_path);

    QJsonObject request_data_obj;
    request_data_obj[QStringLiteral("message")] = QString("Delete: %1").arg(github_path);
    request_data_obj[QStringLiteral("sha")] = sha;
    QJsonDocument doc(request_data_obj);
    auto header = PrepareCommonHeaders(info_.gh_token);
    auto reply = NetworkAccess::Delete(QUrl(url_str), header, doc.toJson(QJsonDocument::Compact));

    msg = QString::fromUtf8(reply.data);
    if (reply.error == QNetworkReply::NoError) {
        auto &list = images_[path];
        for (int i = 0; i < list.size(); i++) {
            if (list.at(i).github_path == github_path) {
                list.removeAt(i);
                break;
            }
        }

        return true;
    } else {
        return false;
    }
}

void GitHubImageHost::SaveConfig()
{
    QFile file(gothubinfo_cfg_);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << info_;
        file.close();
    }
}

void GitHubImageHost::LoadConfig()
{
    QFile file(gothubinfo_cfg_);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> info_;
        file.close();
    }
    qDebug() << info_;
    ReadImages();
}

void GitHubImageHost::WriteImages()
{

    QFile file(image_map_cfg_);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << info_;
        out << int(images_.size());
        foreach (auto key, images_.keys()) {
            auto &list = images_.value(key);
            out << key << int(list.size());
            foreach (auto var, list) {
                out << var;
            }
        }
        file.close();
    }
}

void GitHubImageHost::ReadImages()
{
    images_.clear();
    QFile file(image_map_cfg_);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);

        GithubInfo info;
        in >> info;
        if (info == info_) {
            int path_size;
            in >> path_size;
            while (path_size > 0) {
                path_size--;
                QString path;
                QList<GHImage> img_list;
                int img_size;
                in >> path >> img_size;
                while (img_size > 0) {
                    img_size--;
                    QString path;
                    GHImage img;
                    in >> img;
                    img_list << img;
                }
                images_[path] = img_list;
            }

        } else {
            qDebug() << u8"githubinfo.cfg 校验失败";
        }

        file.close();
    }
}

void GitHubImageHost::ParseConfig(const GithubInfo &jobj,
                                  QString &token,
                                  QString &user_name,
                                  QString &repo_name)
{
    token = jobj.gh_token;
    user_name = jobj.user_name;
    repo_name = jobj.repo_name;
}

NetworkAccess::RawHeaderPairs GitHubImageHost::PrepareCommonHeaders(const QString &token)
{
    using pair = QPair<QByteArray, QByteArray>;
    NetworkAccess::RawHeaderPairs header;
    pair authorization(QByteArray("Authorization"), "token " + token.toUtf8());
    pair accept(QByteArray("Accept"), QByteArray("application/vnd.github.v3+json"));
    header.push_back(authorization);
    header.push_back(accept);
    return header;
}

QStringList GitHubImageHost::QueryImage(const QString &path)
{
    const auto url_str = QUrl(QString("%1/repos/%2/%3/contents/%4").arg(gh_api_url_, info_.user_name, info_.repo_name, path));
    auto reply = NetworkAccess::Request(QUrl(url_str));
    auto recursive_path = ParsingJson(reply, path);
    return recursive_path;
}

QStringList GitHubImageHost::ParsingJson(NetworkReply net_r, const QString &dir)
{
    QStringList dirs;
    auto json_doc = QJsonDocument::fromJson(net_r.data);
    if (json_doc.isArray()) {
        auto json_array = json_doc.array();
        QJsonArray::Iterator i = json_array.begin();
        while (i != json_array.end()) {
            if (i->isObject()) {
                auto tmp_json_obj = i->toObject();
                const auto type = tmp_json_obj[QStringLiteral("type")].toString();
                const auto path = tmp_json_obj[QStringLiteral("path")].toString();
                if ("dir" == type) {
                    dirs << path;
                } else if ("file" == type) {
                    const auto name = tmp_json_obj[QStringLiteral("name")].toString();
                    if (name.right(6).contains("bmp")
                        || name.right(6).contains("gif")
                        || name.right(6).contains("jpg")
                        || name.right(6).contains("jpeg")
                        || name.right(6).contains("png")) {
                        GHImage image;
                        image.html_url = tmp_json_obj[QStringLiteral("html_url")].toString();
                        image.download_url = tmp_json_obj[QStringLiteral("download_url")].toString();
                        image.name = name;
                        image.github_path = tmp_json_obj[QStringLiteral("path")].toString();
                        image.sha = tmp_json_obj[QStringLiteral("sha")].toString();
                        images_[dir] << image;
                    }
                }
            }
            i++;
        }
    }
    return dirs;
}

void GitHubImageHost::DownloadImage(GHImage &gh_image)
{
    if (gh_image.pix.isNull()) {

        auto reply = NetworkAccess::Request(QUrl(gh_image.download_url));

        if (!gh_image.html_url.right(5).contains("gif")) {
            gh_image.pix = QImage::fromData(reply.data).scaled(img_size_, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            QBuffer buffer;
            buffer.open(QFile::WriteOnly);
            buffer.write(reply.data);
            buffer.close();
            QImageReader reader(&buffer);
            gh_image.pix = reader.read().scaled(img_size_, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
}
