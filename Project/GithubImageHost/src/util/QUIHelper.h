#ifndef QUIHELPER_H
#define QUIHELPER_H

#include <QEvent>
#include <QObject>
#include <QScopedPointer>
#include <QMutex>

class QUIHelper : public QObject
{
	Q_OBJECT
public:
	static QUIHelper* Instance();
	explicit QUIHelper(QObject* parent = nullptr);

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

public slots:
	void Start();

public:
	static void SetStyle(QWidget* widget = nullptr, const QString& qssFile = ":/style.qss");

};

#endif // QUIHELPER_H
