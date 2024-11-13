// https://github.com/Greedysky/TTKWidgetTools

#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QEventLoop>

#include "Label.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QPushButton wid;
	wid.setFixedSize(600, 400);

	QObject::connect(&wid, &QPushButton::clicked, &wid, [&wid](){
		QEventLoop loop;

		Label lab;
		lab.SetText(u8"窗前明月光");
		loop.connect(&lab, &Label::SgnFinished, &loop, &QEventLoop::quit);

		lab.Popup(&wid);
		loop.exec();
	});


	wid.show();


	int res = app.exec();

	return res;
}
