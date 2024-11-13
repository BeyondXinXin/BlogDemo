#include <QApplication>

#include "PictureBlockSelectionWidget.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	PictureBlockSelectionWidget* wid1 =
		new PictureBlockSelectionWidget();
	PictureBlockSelectionWidget* wid2 =
		new PictureBlockSelectionWidget();

	wid1->SetBreakGrandImage(QImage(":/pic/1.bmp"));
	wid2->SetBreakGrandImage(QImage(":/pic/1.bmp"));

	wid1->AddBlock(
		QList<QPoint>() << QPoint(12, 12) << QPoint(12, 187)
		                << QPoint(234, 122) << QPoint(122, 12),
		        u8"测试1");

	wid1->AddBlock(
		QList<QPoint>() << QPoint(222, 222) << QPoint(222, 322)
		                << QPoint(456, 300) << QPoint(322, 200),
		        u8"测试2");

	wid2->AddBlock(
		QList<QPoint>() << QPoint(12, 12) << QPoint(12, 245)
		                << QPoint(122, 122) << QPoint(122, 12),
		        u8"测试1");

	wid2->AddBlock(
		QList<QPoint>() << QPoint(444, 444) << QPoint(444, 555)
		                << QPoint(555, 555) << QPoint(555, 444),
		        u8"测试2");

	QObject::connect(wid1, &PictureBlockSelectionWidget::SgnCurrentBlockChange,
	                 wid2, &PictureBlockSelectionWidget::SetCurrentBlock);
	QObject::connect(wid2, &PictureBlockSelectionWidget::SgnCurrentBlockChange,
	                 wid1, &PictureBlockSelectionWidget::SetCurrentBlock);
	QObject::connect(wid1, &PictureBlockSelectionWidget::SgnHoverBlockChange,
	                 wid2, &PictureBlockSelectionWidget::SetHoverBlock);
	QObject::connect(wid2, &PictureBlockSelectionWidget::SgnHoverBlockChange,
	                 wid1, &PictureBlockSelectionWidget::SetHoverBlock);

	wid1->setFixedSize(1064, 1064);
	wid2->setFixedSize(1064, 1064);
	wid1->move(10, 10);
	wid2->move(1200, 10);
	wid1->show();
	wid2->show();

	return QApplication::exec();
}
