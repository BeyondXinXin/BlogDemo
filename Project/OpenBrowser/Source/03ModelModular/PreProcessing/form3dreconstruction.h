#ifndef FORM3DRECONSTRUCTION_H
#define FORM3DRECONSTRUCTION_H

// 01 Frame includes
#include <stable.h>

namespace Ui {
    class Form3DReconstruction;
}

class Form3DReconstruction : public QDialog {
    Q_OBJECT

  public:
    explicit Form3DReconstruction(QWidget *parent = nullptr);
    ~Form3DReconstruction();
    QString GetForm3DReconstructionValue();

  private slots:
    void on_reconstruction_path_btn_clicked();

  private:
    Ui::Form3DReconstruction *ui;
    QString reconstruction_value_;

};

#endif // FORM3DRECONSTRUCTION_H
