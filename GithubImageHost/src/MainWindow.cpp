#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "widget/ThumbnailLabel.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QJsonObject>
#include <QMessageBox>

const QString MainWindow::m_GithubInfoCfg = "githubinfo.cfg";

MainWindow::MainWindow(QWidget* parent)
	: QWidget(parent)
	, m_UI(new Ui::MainWindow)
{
	m_UI->setupUi(this);
	InitUi();
	InitConnect();
}

MainWindow::~MainWindow()
{
	delete m_UI;
}

void MainWindow::InitUi()
{
	setFixedSize(1320, 800);
	setProperty("form", true);
	setProperty("canMove", true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
	m_UI->title_lab->setText("GitHub Image Host");
	m_UI->btnConfigure->setProperty("icoName", "positiontest");
	m_UI->btnUpload->setProperty("icoName", "infoextend");
	m_UI->btnBrowse->setProperty("icoName", "gametest");
	m_UI->panelWidBrowse->SetMargin(10);
	m_UI->panelWidBrowse->SetSpace(10);
	m_UI->stacked->setCurrentIndex(0);

	m_ImageHost.LoadGHInfoConfig();
	GHInfo info = m_ImageHost.GetGHInfo();
	m_UI->lineedit_token->setText(info.ghToken);
	m_UI->lineedit_user_name->setText(info.userName);
	m_UI->lineedit_repo_name->setText(info.repoName);

	auto& images = m_ImageHost.m_GHImageMap;
	m_UI->path_combobox->clear();
	m_UI->path_combobox->addItems(images.keys());
	
	BrowserChangePath(images.keys().first());
	UploadChangePath(QDate::currentDate().toString("yyyyMM"));

	m_UI->labUploadDir->setText(QDate::currentDate().toString("yyyyMM"));
	m_UI->labUploadName->setText(info.userName);
}

void MainWindow::InitConnect()
{
	auto btns = m_UI->title_wid->findChildren<QPushButton*>();
	foreach(auto * btn, btns) {
		connect(btn, &QPushButton::clicked, this, &MainWindow::TitleBtnClicked);
	}

	auto tool_btns = m_UI->control_wid->findChildren<QToolButton*>();
	foreach(auto * btn, tool_btns) {
		btn->setMaximumHeight(80);
		connect(btn, &QToolButton::clicked, this, &MainWindow::ControlBtnClicked);
	}

	connect(m_UI->btnConfigTest, &QPushButton::clicked, this, &MainWindow::SlotConfigBtnClick);
	connect(m_UI->btnConfigConfirm, &QPushButton::clicked, this, &MainWindow::SlotConfigBtnClick);
	connect(m_UI->btnBrowseUpdate, &QPushButton::clicked, this, &MainWindow::SlotBrowseBtnClicked);
	connect(m_UI->path_combobox, &QComboBox::currentTextChanged, this, &MainWindow::BrowserChangePath);
}

void MainWindow::TitleBtnClicked()
{
	const auto* b = qobject_cast<QPushButton*>(sender());
	if (b == m_UI->btnClose) {
		close();
	}
	else if (b == m_UI->btnMin) {
		showMinimized();
	}
}

void MainWindow::ControlBtnClicked()
{
	const auto* b = qobject_cast<QToolButton*>(sender());
	auto btns = m_UI->control_wid->findChildren<QToolButton*>();
	foreach(auto * btn, btns) {
		const auto icoName = btn->property("icoName").toString();
		if (btn != b) {
			btn->setChecked(false);
			btn->setIcon(QIcon(QString(":/image/%1.png").arg(icoName)));
		}
		else {
			btn->setChecked(true);
			btn->setIcon(QIcon(QString(":/image/%1_focus.png").arg(icoName)));
		}
	}

	if (b == m_UI->btnConfigure) {
		m_UI->stacked->setCurrentIndex(0);
	}
	else if (b == m_UI->btnUpload) {
		m_UI->stacked->setCurrentIndex(1);
	}
	else if (b == m_UI->btnBrowse) {
		m_UI->stacked->setCurrentIndex(2);
	}
}

void MainWindow::SlotConfigBtnClick()
{
	const auto* b = qobject_cast<QPushButton*>(sender());

	GHInfo jobj;
	jobj.ghToken = m_UI->lineedit_token->text();
	jobj.userName = m_UI->lineedit_user_name->text();
	jobj.repoName = m_UI->lineedit_repo_name->text();
	if (b == m_UI->btnConfigTest) {
		QString msg;
		if (m_ImageHost.TestConfig(jobj, msg)) {
			QMessageBox::information(nullptr, "Title", u8"测试成功");
			m_UI->lineedit_user_name->setText(jobj.userName);
		}
		else {
			QMessageBox::information(nullptr, "Title", QString(u8"测试失败：%1").arg(msg));
		}
	}
	else if (b == m_UI->btnConfigConfirm) {
		m_ImageHost.SetGHInfo(jobj);
	}
}

void MainWindow::SlotBrowseBtnClicked()
{
	const auto* b = qobject_cast<QPushButton*>(sender());
	if (b == m_UI->btnBrowseUpdate) {
		QString msg;
		if (m_ImageHost.GetAllImage(/*msg*/)) {
			auto& images = m_ImageHost.m_GHImageMap;
			m_UI->path_combobox->clear();
			m_UI->path_combobox->addItems(images.keys());
			m_ImageHost.SaveGHImageMapCache();
		}
		else {
			QMessageBox::information(nullptr, "Title", QString(u8"更新失败：%1").arg(msg));
		}
	}
}

void MainWindow::BrowserChangePath(const QString& path)
{
	qDeleteAll(m_BrowserImgFrms);
	m_BrowserImgFrms.clear();

	auto& images = m_ImageHost.m_GHImageMap.value(path);

	foreach(auto var, images) {
		ThumbnailLabel* lab = new ThumbnailLabel(var);
		m_BrowserImgFrms.append(lab);
		connect(lab, &ThumbnailLabel::SgnDeleteImg,
			this, &MainWindow::DeleteImage, Qt::QueuedConnection);
	}

	m_UI->panelWidBrowse->SetWidget(m_BrowserImgFrms, 5);
}

void MainWindow::UploadChangePath(const QString& path)
{
	qDeleteAll(m_UploadImgFrms);
	m_UploadImgFrms.clear();

	auto& images = m_ImageHost.m_GHImageMap.value(path);

	foreach(auto var, images) {
		ThumbnailLabel* lab = new ThumbnailLabel(var);
		m_UploadImgFrms.append(lab);
		connect(lab, &ThumbnailLabel::SgnDeleteImg,
			this, &MainWindow::DeleteImage, Qt::QueuedConnection);
	}
	m_UI->panelWidUpload->SetWidget(m_UploadImgFrms, 1);
}

void MainWindow::DeleteImage(const QString& githubPath, const QString& sha)
{
	QString msg;
	const auto path = m_UI->path_combobox->currentText();
	GHImage ghImage;
	ghImage.githubPath = githubPath;
	ghImage.sha = sha;
	if (m_ImageHost.RemoveImage(ghImage)) {
		BrowserChangePath(path);
	}
	else {
		QMessageBox::information(nullptr, "Title", QString(u8"删除失败：%1").arg(msg));
	}
}
