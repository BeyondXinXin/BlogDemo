// 01 Frame includes
#include "formmain.h"
#include "ui_formmain.h"
#include "quimenu.h"
#include "formmaskwidget.h"
#include "quihelper.h"
#include "quiiconhelper.h"

FormMain::FormMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMain) {
    ui->setupUi(this);
    initFrom();
}

FormMain::~FormMain() {
    delete ui;
}

void FormMain::initFrom() {
    SlotSetMainWindos(4);

    // modle 初始化
    mode_manager_ = new STLManager(*ui->mainwindow1, this);
    connect(ui->left_form, &FormLeft::SignalsModeBrowserOut, // Mode Handle操作
            this->mode_manager_, &STLManager::SlotPolyDataHandle);
    connect(this->mode_manager_, &STLManager::SignalPromptInformationOut,// 信息
            ui->left_form, &FormLeft::SlotPromptInformation);
    connect(this->mode_manager_, &STLManager::SingnalFinished,// Handle操作完成
            ui->left_form, &FormLeft::SlotModeBrowserBtnEnabledTrue);

    // Image 初始化
    image_manager_ = new ImageBrowserManager(*ui->image_window, this);
    connect(ui->left_form, &FormLeft::SingalImageBrowserOut, // Image Handle操作
            this->image_manager_, &ImageBrowserManager::SlotImgProcess);

    connect(this->image_manager_, &ImageBrowserManager::SignalPromptInformationOut,
            ui->left_form, &FormLeft::SlotPromptInformation); // 信息

    // dcm 初始化
    dcm_manager_ = new DcmManager(ui->view1, ui->view2, ui->view3, ui->view4, this);
    QList<QPushButton *> btns =
        ui->mainwindow2->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setCheckable(true);
        QUIIconHelper::Instance()->SetIcon(
            btn, 0xf009, static_cast<quint32>(btn->size().height() - 10));
        connect(btn, &QPushButton::toggled,
                dcm_manager_, &DcmManager::SlotViewMaximization);
    }

    // images 初始化
    images_manager_ = new ImagesBrowerManager(this);
    ui->images_layout1->addWidget(images_manager_->GetImagesView1());
    ui->images_layout2->addWidget(images_manager_->GetImagesView2());
    connect(ui->left_form, &FormLeft::SignalImagesBrowserOut, // Image Handle操作
            this->images_manager_, &ImagesBrowerManager::SlotImgProcess);

    // ui界面初始化
    connect(ui->left_form, &FormLeft::SingalSliderBarMovtToOut,
            this, &FormMain::SlotSetMainWindos);
    connect(this, &FormMain::SignalMainWindosChangeOut,
            this, &FormMain::SlotSetMainWindos);
    connect(this, &FormMain::SignalMainWindosChangeOut,
            ui->left_form, &FormLeft::SingalSliderBarMovtToIn);

}


void FormMain::SlotOpenFileIn(QString tmp_file) {
    if (tmp_file.isEmpty()) {
        tmp_file =
            QUIHelper::GetFileName("*.dcm "
                                   "*.stl *.ply *.vtkxml *.vtp *.vtk "
                                   "*.bmp *.jpg *.pbm *.pgm "
                                   "*.png *.ppm *.xbm *.xpm ");
    }
    QFileInfo file_info(tmp_file);
    QString types = file_info.suffix();
    if (types == "dcm") {
        dcm_manager_->OpenStlFile(tmp_file);
        SlotSetMainWindos(2);
    } else if (types == "stl" ||
               types == "ply" ||
               types == "vtkxml" ||
               types == "vtp" ||
               types == "vtk"  ) {
        mode_manager_->OpenModelFile(tmp_file);
        SlotSetMainWindos(1);
    } else if (types == "bmp" ||
               types == "jpg" ||
               types == "pbm" ||
               types == "pgm" ||
               types == "png" ||
               types == "ppm" ||
               types == "xbm" ||
               types == "xpm") {
        image_manager_->OpenStlFile(tmp_file);
        SlotSetMainWindos(0);
    }
}

void FormMain::SlotSetMainWindos(const int mainwindow) {
    ui->main_stacked_widget->setCurrentIndex(mainwindow);
}

















