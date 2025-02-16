#include "widget.h"

#include <QApplication>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QMessageBox>

int main(int argc, char * argv[])
{
    QCoreApplication::setOrganizationName("BeyondXin");
    QCoreApplication::setApplicationName("SonicRhythm");

    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, 1);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, 1);
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication a(argc, argv);

    Widget w;
    w.setWindowTitle(u8"SonicRhythm (Powered By Beyond欣)");
    w.resize(800, 600);
    w.show();

    return a.exec();
}
