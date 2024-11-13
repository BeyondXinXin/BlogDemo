#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QItemSelection>
#include <QMenu>
#include <QPointer>
#include <QPushButton>

#include "util/GitHubImageHost.h"

class ThumbnailLabel : public QWidget
{
	Q_OBJECT

public:
	explicit ThumbnailLabel(const GHImage& img, QWidget* parent = nullptr);
	~ThumbnailLabel() override;

Q_SIGNALS:
	void SgnDeleteImg(const QString& github_path, const QString& sha);

protected:
	void paintEvent(QPaintEvent*) override;
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;

	virtual void contextMenuEvent(QContextMenuEvent* e) override;

private:
	void Initialize();
	void DrawBackground(QPainter* painter);

private:
	GHImage m_GHImage;
	bool m_Hover{ false };

	QPointer<QMenu> m_ContextMenu;
	QItemSelection m_Selection;
};

#endif // THUMBNAILLABEL_H
