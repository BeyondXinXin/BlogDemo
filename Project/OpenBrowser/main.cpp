// 01 Frame includes
#include "commandlinemanager.h"
#include "openbrowserapplication.h"
#include "quiapplication.h"
#include "quihelper.h"

// VTK includes
#include <QVTKOpenGLWidget.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOutputWindow.h>

#include "qopencvwidget.h"

int main(int argc, char *argv[])
{
    // 主线程名称
    QThread::currentThread()->setObjectName("Main Thread");

    // 设置window渲染方式
    vtkOutputWindow::SetGlobalWarningDisplay(0);
    vtkOpenGLRenderWindow::SetGlobalMaximumNumberOfMultiSamples(8);
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

    // 信号槽传递类型注册
    // qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");

    // qt全局初始化
    QUIApplication a(argc, argv);
    if (!a.lock()) {
        QUIHelper::ShowMessageBoxError("正在运行，请不要重复启动", 5, true);
        return 0;
    }

    QOpencvScene opencv_scene_;
    QOpencvView graphicsView;
    QOpencvPixmapItem opencv_pixmap_;
    opencv_scene_.addItem(&opencv_pixmap_);
    graphicsView.setScene(&opencv_scene_);
    opencv_pixmap_.setPixmap(QPixmap("C:\\Users\\Administrator\\Pictures\\视频项目\\288.png"));
    graphicsView.setDragMode(QGraphicsView::ScrollHandDrag);
    graphicsView.show();

    //    // 命令行参数设置
    //    CommandLineManager::Initial(a);

    //    // 启动主程序
    //    openBrowserApplication openBrowser_application;
    //    if (openBrowser_application.Run()) {
    //    } else {
    //        return 0;
    //    }
    return a.exec();
}
