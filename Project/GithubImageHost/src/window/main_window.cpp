#include "main_window.h"
#include "ui_main_window.h"

#include "widget/thumbnail_label.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QJsonObject>
#include <QMessageBox>

const QString MainWindow::gothubinfo_cfg_ = "githubinfo.cfg";

MainWindow::MainWindow(QWidget *parent)
  : QWidget(parent)
  , ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    InitUi();
    InitConnect();
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::InitUi()
{
    setFixedSize(1320, 800);
    this->setProperty("form", true);
    this->setProperty("canMove", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    ui_->title_lab->setText("GitHub Image Host");
    ui_->btn_configure->setProperty("icoName", "positiontest");
    ui_->btn_upload->setProperty("icoName", "infoextend");
    ui_->btn_browse->setProperty("icoName", "gametest");
    ui_->panel_wid->SetMargin(10);
    ui_->panel_wid->SetSpace(10);
    ui_->stacked->setCurrentIndex(0);

    image_host_.LoadConfig();
    if (!image_host_.info_.isEmpty()) {
        ui_->lineedit_token->setText(image_host_.info_.gh_token);
        ui_->lineedit_user_name->setText(image_host_.info_.user_name);
        ui_->lineedit_repo_name->setText(image_host_.info_.repo_name);
    }
    auto &images = image_host_.images_;
    ui_->path_combobox->clear();
    ui_->path_combobox->addItems(images.keys());
    BrowserChangePath(images.keys().first());
}

void MainWindow::InitConnect()
{
    auto btns = ui_->title_wid->findChildren<QPushButton *>();
    foreach (auto *btn, btns) {
        connect(btn, &QPushButton::clicked, this, &MainWindow::TitleBtnClicked);
    }

    auto tool_btns = ui_->control_wid->findChildren<QToolButton *>();
    foreach (auto *btn, tool_btns) {
        btn->setMaximumHeight(80);
        connect(btn, &QToolButton::clicked, this, &MainWindow::ControlBtnClicked);
    }

    connect(ui_->btn_test_config, &QPushButton::clicked, this, &MainWindow::ConfigureBtnClicked);
    connect(ui_->btn_set_config, &QPushButton::clicked, this, &MainWindow::ConfigureBtnClicked);

    connect(ui_->btn_load_image, &QPushButton::clicked, this, &MainWindow::BrowseBtnClicked);

    connect(ui_->path_combobox, &QComboBox::currentTextChanged, this, &MainWindow::BrowserChangePath);
}

void MainWindow::TitleBtnClicked()
{
    const auto *b = qobject_cast<QPushButton *>(sender());
    if (b == ui_->btn_close) {
        close();
    } else if (b == ui_->btn_max) {
        static bool max = false;
        static QRect location = this->geometry();
        if (max) {
            this->setGeometry(location);
        } else {
            location = this->geometry();
            this->setGeometry(qApp->desktop()->availableGeometry());
        }
        this->setProperty("canMove", max);
        max = !max;
    } else if (b == ui_->btn_min) {
        showMinimized();
    }
}

void MainWindow::ControlBtnClicked()
{
    const auto *b = qobject_cast<QToolButton *>(sender());
    auto btns = ui_->control_wid->findChildren<QToolButton *>();
    foreach (auto *btn, btns) {
        const auto icoName = btn->property("icoName").toString();
        if (btn != b) {
            btn->setChecked(false);
            btn->setIcon(QIcon(QString(":/image/%1.png").arg(icoName)));
        } else {
            btn->setChecked(true);
            btn->setIcon(QIcon(QString(":/image/%1_focus.png").arg(icoName)));
        }
    }

    if (b == ui_->btn_configure) {
        ui_->stacked->setCurrentIndex(0);
    } else if (b == ui_->btn_upload) {
        ui_->stacked->setCurrentIndex(1);
    } else if (b == ui_->btn_browse) {
        ui_->stacked->setCurrentIndex(2);
    }
}

void MainWindow::ConfigureBtnClicked()
{
    const auto *b = qobject_cast<QPushButton *>(sender());

    GithubInfo jobj;
    jobj.gh_token = ui_->lineedit_token->text();
    jobj.user_name = ui_->lineedit_user_name->text();
    jobj.repo_name = ui_->lineedit_repo_name->text();
    if (b == ui_->btn_test_config) {
        QString msg;
        if (image_host_.TestConfig(jobj, msg)) {
            QMessageBox::information(nullptr, "Title", u8"      测试成功");
        } else {
            QMessageBox::information(nullptr, "Title", u8"      测试失败：" + msg);
        }
    } else if (b == ui_->btn_set_config) {
        image_host_.info_ = jobj;
        image_host_.SaveConfig();
    }
}

void MainWindow::BrowseBtnClicked()
{
    const auto *b = qobject_cast<QPushButton *>(sender());
    if (b == ui_->btn_load_image) {
        QString msg;
        if (image_host_.GetAllImage(msg)) {
            auto &images = image_host_.images_;
            ui_->path_combobox->clear();
            ui_->path_combobox->addItems(images.keys());

        } else {
            QMessageBox::information(nullptr, "Title", u8"      更新失败：" + msg);
        }
    }
}

void MainWindow::BrowserChangePath(const QString &path)
{
    qDeleteAll(img_frms_);
    img_frms_.clear();

    auto &images = image_host_.images_.value(path);

    foreach (auto var, images) {
        ThumbnailLabel *lab = new ThumbnailLabel(var);
        img_frms_.append(lab);
        // 信号发送者自身会被销毁
        connect(lab, &ThumbnailLabel::SgnDeleteImg, this, &MainWindow::DeleteImage, Qt::QueuedConnection);
    }

    ui_->panel_wid->SetWidget(img_frms_, 5);
}

void MainWindow::DeleteImage(const QString &github_path, const QString &sha)
{
    QString msg;
    const auto path = ui_->path_combobox->currentText();
    if (image_host_.Remove(path, github_path, sha, msg)) {
        image_host_.WriteImages();
        BrowserChangePath(path);
    } else {
        QMessageBox::information(nullptr, "Title", u8"      删除失败：" + msg);
    }
}
