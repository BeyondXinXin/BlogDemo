#ifndef QUIICONHELPER_H
#define QUIICONHELPER_H

/*
 * 图形字体处理类
*/

// 01 Frame includes
#include "stable.h"

class QUIIconHelper : public QObject {
    Q_OBJECT

  public:
    static QUIIconHelper *Instance();
    explicit QUIIconHelper(QObject *parent = nullptr);
    QFont GetIconFont(); //获取图形字体
    void SetIcon(QLabel *lab, const QChar &str, quint32 size = 12);//设置图形字体到标签
    void SetIcon(QAbstractButton *btn,
                 const QChar &str, quint32 size = 12);//设置图形字体到按钮
    QPixmap GetPixmap(
        const QColor &color, const QChar &str, quint32 size = 12,
        quint32 pixWidth = 15, quint32 pixHeight = 15,
        int flags = Qt::AlignCenter);//获取指定图形字体,可以指定文字大小,图片宽高,文字对齐
    QPixmap GetPixmap(QToolButton *btn, bool normal);//根据按钮获取该按钮对应的图标
  protected:
    bool eventFilter(QObject *watched, QEvent *event);
  private:
    static QScopedPointer<QUIIconHelper> self_;

    QFont icon_font_;             //图形字体
    QList<QToolButton *> btns_;  //按钮队列
    QList<QPixmap> pix_normal_;   //正常图片队列
    QList<QPixmap> pix_dark_;     //加深图片队列
};

#endif // QUIICONHELPER_H
