#include <QApplication>
#include <QFile>
#include <QTextCodec>

#include "util/QUIHelper.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	a.setFont(QFont("Microsoft Yahei", 9));

	QUIHelper::Instance()->Start();
	QUIHelper::SetStyle();

	MainWindow w;
	w.show();

	return a.exec();
}