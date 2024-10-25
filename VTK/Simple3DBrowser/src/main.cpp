// 使用中文需要源码自身编码为 UTF-8
// #pragma execution_character_set("utf-8")

#include <QtWidgets>
#include <S3DBrowser>

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    QWidget mainWidget;

    S3DWidget::Options options;
    options.activateShortcutKeys = true;
    S3DWidget s3dWid(&mainWidget);
    s3dWid.SetOptions(options);
    s3dWid.setMinimumSize(800, 600);

    QLabel label("G: Grid  A: Axis  C: CameraOrientation \nE: Edge  S: SSAO  T: ToneMapping \nLeft/Reght", &mainWidget);
    label.setFixedSize(800, 50);

    QVBoxLayout * layout = new QVBoxLayout(&mainWidget);
    layout->addWidget(&s3dWid);
    layout->addWidget(&label);
    mainWidget.setLayout(layout);
    mainWidget.show();

    QTimer::singleShot(0, [&s3dWid] {
        s3dWid.SetObjFiles({ u8"F:/1.obj", u8"F:/中文测试.obj", u8"F:/错误文件测试.obj", u8"F:/错误格式测试.stl", u8"空白测试" });
    });

    return app.exec();
}
