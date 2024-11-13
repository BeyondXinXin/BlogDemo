#include <QApplication>

#include "DoubleSlider.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	auto slider = new DoubleSlider(-5000, 10000);
	slider->setFixedHeight(50);
	slider->show();
	return QApplication::exec();
}
