#ifndef APPINIT_H
#define APPINIT_H

#include <QEvent>
#include <QObject>

class QUIHelper : public QObject
{
    Q_OBJECT
public:
    static QUIHelper *Instance();
    explicit QUIHelper(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    static QScopedPointer<QUIHelper> self;

public slots:
    void start();

public:
    static void SetStyle(QWidget *wid = nullptr,
                         const QString &qssFile = ":/style.qss");
};

#endif // APPINIT_H
