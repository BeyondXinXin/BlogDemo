#ifndef QUIHELPER_H
#define QUIHELPER_H

#include <QObject>

class QUIHelper : public QObject
{
    Q_OBJECT
public:
    static void SetStyle(
      const QString& qss_file = ":/qss/Style/style.qss"); // 设置全局样式
    static void SetCode(); // 设置编码
};

#endif // QUIHELPER_H
