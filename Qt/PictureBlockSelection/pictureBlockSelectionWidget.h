#ifndef SPLITCONESKETCHWIDGET_H
#define SPLITCONESKETCHWIDGET_H

#include <QWidget>

/**
 * @brief The PictureBlockSelectionWidget class
 * 单纯的图片分割成几个模块，增加移入、选中效果的widget。
 * 注意：同时存在两个时，判断当前鼠标距离那个block中心更近比较麻烦，QHash存储路径，出现这种情况就听天由命吧，用最后找到的那个
 */
class PictureBlockSelectionWidget : public QWidget {
	Q_OBJECT

public:

	explicit PictureBlockSelectionWidget(QWidget* parent = nullptr);
	~PictureBlockSelectionWidget() override;
	void AddBlock(const QList<QPoint>& path, const QString& name);
	void SetCurrentBlock(const int& id);
	void SetHoverBlock(const int& id);
	void ClearAllBlock();
	void SetBreakGrandImage(const QImage& img);
	static qreal GetImageSizeWidth();
	static qreal GetImageSizeHeigh();

Q_SIGNALS:

	void SgnCurrentBlockChange(const int& cur_block);
	void SgnHoverBlockChange(const int& cur_block);

protected:

	void paintEvent(QPaintEvent *) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

private:

	void Initial();
	void DrawBg(QPainter* painter);
	void DrawBgImage(QPainter* painter);
	void DrawFrame(QPainter* painter);
	void DrawSelectFrame(QPainter* painter);
	void DrawSelectBlock(QPainter* painter);
	void DrawBlockName(QPainter* painter);

private:

	int m_HoverBlock;                          // 悬停ID
	int m_CurBlock;                            // 选中ID
	QHash<int, QList<QPoint> >m_BlockPathHash; // 块路径(只允许重置)
	QStringList m_TagList;
	QImage m_BgImage;                          // 背景图片
};

#endif // SPLITCONESKETCHWIDGET_H
