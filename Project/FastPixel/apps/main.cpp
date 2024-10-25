#include <QApplication>

#include <QWidget>

#include "main_window.h"
#include "palette/palette_manager.h"
#include "qui_helper.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //    QUIHelper::SetStyle();
    QUIHelper::SetCode();

    MainWindow* win = new MainWindow();
    win->Launch();

    return a.exec();
}
