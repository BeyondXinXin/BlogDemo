#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "util/githubimagehost.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

private:
	void InitUi();
	void InitConnect();

	void SlotConfigBtnClick();
	void TitleBtnClicked();
	void ControlBtnClicked();
	void SlotBrowseBtnClicked();

	void BrowserChangePath(const QString& path);
	void UploadChangePath(const QString& path);

private:
	void DeleteImage(const QString& githubPath, const QString& sha);

private:
	Ui::MainWindow* m_UI;
	GitHubImageHost m_ImageHost;
	QList<QWidget*> m_BrowserImgFrms;
	QList<QWidget*> m_UploadImgFrms;
	const static QString m_GithubInfoCfg;
};

#endif // MAINWINDOW_H
