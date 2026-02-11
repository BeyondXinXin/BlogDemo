// 01 Frame includes
#include "form3dreconstruction.h"
#include "ui_form3dreconstruction.h"
#include "quihelper.h"

Form3DReconstruction::Form3DReconstruction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form3DReconstruction) {
    ui->setupUi(this);
}

Form3DReconstruction::~Form3DReconstruction() {
    delete ui;
}

QString Form3DReconstruction::GetForm3DReconstructionValue() {
    reconstruction_value_ = "";
    reconstruction_value_ =
        ui->reconstruction_path->text() + "|" +
        ui->reconstruction_savrpath->text() + "|" +
        QString::number(ui->reconstruction_decimatepro->value()) + "|" +
        QString::number(ui->reconstruction_smooth->value()) + "|" +
        ui->reconstruction_type->text() + "|" +
        ui->reconstruction_typename->text() + "|" +
        QString::number(ui->reconstruction_sum_begin->value()) + "|" +
        QString::number(ui->reconstruction_sum_end->value()) + "|" +
        QString::number(ui->reconstruction_spacing_x->value()) + "|" +
        QString::number(ui->reconstruction_spacing_y->value()) + "|" +
        QString::number(ui->reconstruction_spacing_z->value()) + "|" +
        QString::number(ui->reconstruction_cubes->value());
    return reconstruction_value_;
}

void Form3DReconstruction::on_reconstruction_path_btn_clicked() {
    QString file_path = QUIHelper::GetFolderName();
    ui->reconstruction_path->setText(file_path + "/");
    QDir *dir = new QDir(file_path);
    qint32 this_sum_ = 0;
    QList<QFileInfo> *fileInfo =
        new QList<QFileInfo>(dir->entryInfoList());
    for (int i = 0; i < fileInfo->count(); i++) {
        if (fileInfo->at(i).suffix() == ui->reconstruction_type->text()) {
            this_sum_++;
        }
    }
    ui->reconstruction_sum_end->setValue(this_sum_);
}
