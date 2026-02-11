#ifndef MASKWIDGET_H
#define MASKWIDGET_H

/**
 * 遮挡罩控件
 */

// 01 Frame includes
#include "stable.h"

class MaskWidget : public QWidget {
    Q_OBJECT
  public:
    explicit MaskWidget(QWidget *parent = nullptr);

    static MaskWidget *Instance() {
        static QMutex mutex;
        if (!self_) {
            QMutexLocker locker(&mutex);
            if (!self_) {
                self_ = new MaskWidget;
            }
        }
        return self_;
    }
  public slots:
    void SlotSetMainWidget(QWidget *main_widget_);
    void SlotSetDialogNames(const QStringList &dialog_names_);
    void SlotSetBgColor(const QColor &bgColor);
    void SlotSetOpacity(double opacity);

  protected:
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

  private:
    static MaskWidget *self_;
    QWidget *main_widget_;
    QStringList dialog_names_;


};

#endif // MASKWIDGET_H
