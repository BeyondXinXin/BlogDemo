#include <QApplication>

#include "BullsEye.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	BullsEye* bullesEye = new BullsEye();
	bullesEye->setFixedSize(1200, 1000);
	QVector<double> value1;
	value1 << 0.06 << 0.12 << 0.18 << 0.24 << 0.29 << 0.35
	       << 0.41 << 0.47 << 0.53 << 0.59 << 0.65 << 0.71
	       << 0.76 << 0.82 << 0.88 << 0.94 << 1.00;
	QVector<double> value2;
	value2 << 0.76 << 0.82 << 0.88 << 0.94 << 1.00 << 0.06
	       << 0.12 << 0.18 << 0.24 << 0.29 << 0.35
	       << 0.41 << 0.47 << 0.53 << 0.59 << 0.65 << 0.71;
	QVector<double> value3;
	value3 << 0.41 << 0.47 << 0.53 << 0.59 << 0.65 << 0.71
	       << 0.06 << 0.12 << 0.18 << 0.24 << 0.29 << 0.35
	       << 0.76 << 0.82 << 0.88 << 0.94 << 1.00;

	QList<QVector<double> > bullvalue;
	bullvalue << value1 << value2 << value3;
	bullesEye->SetCurrentValue(bullvalue);
	bullesEye->show();

	return a.exec();
}
