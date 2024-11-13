#include "QUIHelper.h"
#include <QFile>
#include <QTextStream>
#include <QMouseEvent>
#include <QApplication>
#include <QWidget>
#include <QDebug>

QUIHelper* QUIHelper::Instance()
{
	static QUIHelper instance;
	return &instance;
}

QUIHelper::QUIHelper(QObject* parent)
	: QObject(parent)
{
}

bool QUIHelper::eventFilter(QObject* watched, QEvent* event)
{
	QWidget* widget = qobject_cast<QWidget*>(watched);
	// 如果 widget 为 null 或者不能移动，直接返回
	if (!widget || !widget->property("canMove").toBool()) {
		return QObject::eventFilter(watched, event);
	}

	static QPoint mousePoint;
	static bool mousePressed = false;

	QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
	// 处理鼠标按下事件
	if (mouseEvent->type() == QEvent::MouseButtonPress) {
		if (mouseEvent->button() == Qt::LeftButton) {
			mousePressed = true;
			mousePoint = mouseEvent->globalPos() - widget->pos();
			return true;
		}
	}
	// 处理鼠标释放事件
	else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
		mousePressed = false;
		return true;
	}
	// 处理鼠标移动事件
	else if (mouseEvent->type() == QEvent::MouseMove) {
		if (mousePressed && (mouseEvent->buttons() == Qt::LeftButton)) {
			widget->move(mouseEvent->globalPos() - mousePoint);
			return true;
		}
	}

	return QObject::eventFilter(watched, event);
}

void QUIHelper::Start()
{
	qApp->installEventFilter(this);
}

void QUIHelper::SetStyle(QWidget* widget, const QString& qssFile)
{
	QFile file(qssFile);
	if (file.open(QFile::ReadOnly)) {
		QString qss;
		QTextStream in(&file);
		qss = in.readAll();
		QString paletteColor = qss.mid(15, 7);
		if (widget) {
			widget->setPalette(QPalette(QColor(paletteColor)));
			widget->setStyleSheet(qss);
			widget->setAutoFillBackground(true);
		}
		else {
			qApp->setPalette(QPalette(QColor(paletteColor)));
			qApp->setStyleSheet(qss);
		}

		file.close();
	}
	else {
		qInfo() << "Open qss file failed!" << qssFile;
	}
}
