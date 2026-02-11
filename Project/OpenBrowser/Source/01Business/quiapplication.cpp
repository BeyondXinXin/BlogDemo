#include "stable.h"
#include "quiapplication.h"
#include "quihelper.h"
#include "quiwidgetmove.h"

// 仅能启动一个实例
QUIApplication::QUIApplication(int &argc, char **argv):
    QApplication(argc, argv, true) {
    _singular = new QSharedMemory("SharedMemory", this);
    this->setWindowIcon(QIcon(":/Image/logo.ico"));
    QFont font;
    font.setFamily("Microsoft Yahei");
    font.setPixelSize(15);
    qApp->setFont(font);
    QUIHelper::SetCode();
    QUIWidgetMove::Instance()->Run();//支持界面拖拽
}

QUIApplication::~QUIApplication() {
    if (_singular->isAttached()) {
        _singular->detach();
    }
}

bool QUIApplication::lock() {
    if (_singular->attach(QSharedMemory::ReadOnly)) {
        _singular->detach();
        return false;
    }
    if (_singular->create(1)) {
        return true;
    }
    return false;
}

