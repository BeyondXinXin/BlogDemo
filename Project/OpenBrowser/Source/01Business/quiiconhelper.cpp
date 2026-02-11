// 01 Frame includes
#include "quiiconhelper.h"


QScopedPointer<QUIIconHelper> QUIIconHelper::self_;
QUIIconHelper *QUIIconHelper::Instance() {
    if (self_.isNull()) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self_.isNull()) {
            self_.reset(new QUIIconHelper);
        }
    }
    return self_.data();
}

QUIIconHelper::QUIIconHelper(QObject *parent) : QObject(parent) {
    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("FontAwesome")) {
        int fontId = fontDb.addApplicationFont(":/Image/fontawesome-webfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load fontawesome-webfont.ttf error";
        }
    }
    if (fontDb.families().contains("FontAwesome")) {
        icon_font_ = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        icon_font_.setHintingPreference(QFont::PreferNoHinting);
#endif
    }
}

QFont QUIIconHelper::GetIconFont() {
    return this->icon_font_;
}

void QUIIconHelper::SetIcon(QLabel *lab, const QChar &str, quint32 size) {
    icon_font_.setPixelSize(static_cast<int>(size));
    lab->setFont(icon_font_);
    lab->setText(str);
}

void QUIIconHelper::SetIcon(QAbstractButton *btn, const QChar &str, quint32 size) {
    icon_font_.setPixelSize(static_cast<int>(size));
    btn->setFont(icon_font_);
    btn->setText(str);
}

QPixmap QUIIconHelper::GetPixmap(const QColor &color, const QChar &str, quint32 size,
                                 quint32 pixWidth, quint32 pixHeight, int flags) {
    QPixmap pix(static_cast<int>(pixWidth), static_cast<int>(pixHeight));
    pix.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(color);
    icon_font_.setPixelSize(static_cast<int>(size));
    painter.setFont(icon_font_);
    painter.drawText(pix.rect(), flags, str);
    painter.end();
    return pix;
}

QPixmap QUIIconHelper::GetPixmap(QToolButton *btn, bool normal) {
    QPixmap pix;
    int index = btns_.indexOf(btn);
    if (index >= 0) {
        if (normal) {
            pix = pix_normal_.at(index);
        } else {
            pix = pix_dark_.at(index);
        }
    }
    return pix;
}

bool QUIIconHelper::eventFilter(QObject *watched, QEvent *event) {
    if (watched->inherits("QToolButton")) {
        QToolButton *btn = static_cast<QToolButton *>(watched);
        int index = btns_.indexOf(btn);
        if (index >= 0) {
            if (event->type() == QEvent::Enter) {
                btn->setIcon(QIcon(pix_dark_.at(index)));
            } else if (event->type() == QEvent::Leave) {
                if (btn->isChecked()) {
                    btn->setIcon(QIcon(pix_dark_.at(index)));
                } else {
                    btn->setIcon(QIcon(pix_normal_.at(index)));
                }
            }
        }
    }
    return QObject::eventFilter(watched, event);
}

