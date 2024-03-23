#ifndef SETTING_DIALOG_H
#define SETTING_DIALOG_H

#include <QDialog>

class QSettings;

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget* parent = nullptr);
    ~SettingDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SettingDialog* ui;
    QSettings* setting_;
};

#endif // SETTING_DIALOG_H
