#include "setting_dialog.h"
#include "ui_setting_dialog.h"

#include <QDebug>
#include <QSettings>

SettingDialog::SettingDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    setting_ = new QSettings(this);

    ui->edit_baidu_id->setText(
      setting_->value("Baidu/appid", QString()).toString());
    ui->edit_baidu_key->setText(
      setting_->value("Baidu/key", QString()).toString());
    ui->cbox_api->setCurrentIndex(setting_->value("Global/api_id", "").toInt());
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_buttonBox_accepted()
{
    setting_->setValue("Baidu/appid", ui->edit_baidu_id->text());
    setting_->setValue("Baidu/key", ui->edit_baidu_key->text());
    setting_->setValue("Global/api_id", ui->cbox_api->currentIndex());

    emit accept();
}
