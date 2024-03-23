#include "qui_helper.h"

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QPalette>
#include <QTextCodec>
#include <QTextStream>

void QUIHelper::SetStyle(const QString& qss_file)
{
    QFile file(qss_file);
    if (file.open(QFile::ReadOnly)) {
        QString qss;
        QStringList list;
        QTextStream in(&file);
        // in.setCodec("utf-8");
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }
        qss = list.join("\n");
        QString paletteColor = qss.mid(15, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void QUIHelper::SetCode()
{
    QTextCodec* codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
}
