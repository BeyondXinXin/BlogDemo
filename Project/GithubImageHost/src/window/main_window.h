#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>

#include "util/github_image_host.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void InitUi();
    void InitConnect();

    void TitleBtnClicked();
    void ControlBtnClicked();
    void ConfigureBtnClicked();
    void BrowseBtnClicked();

    void BrowserChangePath(const QString &path);

private:
    void DeleteImage(const QString &github_path, const QString &sha);

private:
    Ui::MainWindow *ui_;
    GitHubImageHost image_host_;
    QList<QWidget *> img_frms_;

    const static QString gothubinfo_cfg_;
};

#endif // MAIN_WINDOW_H
