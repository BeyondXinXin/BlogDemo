#ifndef GITHUBIMAGEHOST_H
#define GITHUBIMAGEHOST_H

#include "network_access.h"

#include <QDataStream>
#include <QDebug>
#include <QImage>

struct GithubInfo
{
    QString gh_token;
    QString user_name;
    QString repo_name;

    friend QDataStream &operator<<(QDataStream &out, const GithubInfo &info)
    {
        return out << info.gh_token << info.user_name << info.repo_name;
    }

    friend QDataStream &operator>>(QDataStream &in, GithubInfo &info)
    {
        return in >> info.gh_token >> info.user_name >> info.repo_name;
    }

    friend QDebug &operator<<(QDebug out, const GithubInfo &info)
    {
        out << info.gh_token << info.user_name << info.repo_name;
        return out;
    }

    inline bool operator==(const GithubInfo &info)
    {
        return gh_token == info.gh_token && user_name == info.user_name && repo_name == info.repo_name;
    }

    inline bool isEmpty() const
    {
        return gh_token.isEmpty() || user_name.isEmpty() || repo_name.isEmpty();
    }
};

struct GHImage
{
    QString html_url { "" };
    QString download_url { "" };
    QString name { "" };
    QString github_path { "" };
    QString sha { "" };
    QImage pix;

    friend QDataStream &operator<<(QDataStream &out, const GHImage &info)
    {
        return out << info.html_url << info.download_url << info.name << info.github_path << info.sha << info.pix;
    }

    friend QDataStream &operator>>(QDataStream &in, GHImage &info)
    {
        return in >> info.html_url >> info.download_url >> info.name >> info.github_path >> info.sha >> info.pix;
    }

    inline bool operator==(const GHImage &info)
    {
        return sha == info.sha && github_path == info.github_path;
    }
};

class GitHubImageHost : public QObject
{
    Q_OBJECT
public:
    using GHImageMap = QMap<QString, QList<GHImage>>;

    explicit GitHubImageHost(QObject *parent = nullptr);

    bool GetAllImage(QString &msg);
    bool TestConfig(const GithubInfo &jobj, QString &msg);
    QString Create(const QByteArray &data, const QString &path, QString &msg);
    bool Remove(const QString &path, const QString &name, const QString &sha, QString &msg);

    void SaveConfig();
    void LoadConfig();

    void WriteImages();
    void ReadImages();

private:
    static void ParseConfig(const GithubInfo &jobj,
                            QString &token,
                            QString &user_name,
                            QString &repo_name);

    static NetworkAccess::RawHeaderPairs PrepareCommonHeaders(const QString &token);
    static QStringList QueryImage(const QString &path);
    static QStringList ParsingJson(NetworkReply net_r, const QString &dir);
    static void DownloadImage(GHImage &gh_image);

private:
    static const QString gh_api_url_;
    const static QString gothubinfo_cfg_;
    const static QString image_map_cfg_;

public:
    static GHImageMap images_;
    static QSize img_size_;
    static GithubInfo info_;
};

#endif // GITHUBIMAGEHOST_H
