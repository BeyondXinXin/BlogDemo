#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QStyle>
#include <QTextEdit>
#include <SingleApplication>

#include "main_window.h"
#include "qui_trayicon.h"
#include "title_tool_bar.h"

void SetStyle(const QString& qss_file, QWidget* wid = nullptr)
{
    QFile file(qss_file);
    if (file.open(QFile::ReadOnly)) {
        QString qss;
        QStringList list;
        QTextStream in(&file);

        in.setCodec("utf-8");
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }
        qss = list.join("\n");

        QString paletteColor = qss.mid(15, 7);
        if (wid) {
            wid->setPalette(QPalette(QColor(paletteColor)));
            wid->setStyleSheet(qss);
        } else {
            qApp->setPalette(QPalette(QColor(paletteColor)));
            qApp->setStyleSheet(qss);
        }
        file.close();
    }
}

bool LoadFont(const QString& str_path)
{
    QFile file(str_path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    int font_id = QFontDatabase::addApplicationFontFromData(file.readAll());
    if (font_id == -1) {
        return false;
    }

    QStringList font_family = QFontDatabase::applicationFontFamilies(font_id);
    if (font_family.empty()) {
        return false;
    }

    QFont font(font_family.at(0));
    qApp->setFont(font);
    return true;
}

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationName("BeyondXin");
    QCoreApplication::setApplicationName("CodingTranslate");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    SingleApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    LoadFont(":/pingfang.ttf");
    SetStyle(":/style.qss");

    MainWindow* w = new MainWindow;

    QUITrayIcon::Instance()->SetMainWidget(w);
    QUITrayIcon::Instance()->SetIcon(
      QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    QUITrayIcon::Instance()->SlotsetVisible(true);

    return app.exec();
}
