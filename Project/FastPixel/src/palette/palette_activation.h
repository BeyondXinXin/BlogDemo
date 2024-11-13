#ifndef COLORPALETTEACTIVATION_H
#define COLORPALETTEACTIVATION_H

#include "palette_enum.h"

#include <QLabel>
#include <QPushButton>
#include <QWidget>

class PaletteManager;

class PaletteActivation : public QWidget
{
    Q_OBJECT
public:
    PaletteActivation(QWidget* parent = nullptr);
    ~PaletteActivation() override;

    void SetManager(PaletteManager* manager);
    void UpdataCurColor(ColorType c_type);

Q_SIGNALS:
    void SgnAddColor(ColorType c_type);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QColor bg_color_;

    QLabel* primary_lab_;
    QLabel* secondary_lab_;
    QPushButton* primary_btn_;
    QPushButton* secondary_btn_;

    int space_;

    PaletteManager* manager_;
};

#endif // COLORPALETTEACTIVATION_H
