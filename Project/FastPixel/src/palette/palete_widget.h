#ifndef PALETE_WIDGET_H
#define PALETE_WIDGET_H

#include "palette_enum.h"

#include <QWidget>

class PaletteManager;

class PaleteWidget : public QWidget
{
    Q_OBJECT
public:
    PaleteWidget(PaletteManager* manager = nullptr, QWidget* parent = nullptr);
    ~PaleteWidget() override;

    void SetManager(PaletteManager* manager);
    virtual void UpdataCurColor(ColorType c_type);

    virtual void UpdataBgPix() = 0;

Q_SIGNALS:
    void SgnChangeColor(ColorType c_type, const QColor& color);
    void SgnSelectColor(ColorType c_type, const int& color_id);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    virtual void DrawBg(QPainter* painter);
    virtual void DrawDecorate(QPainter* painter) = 0;

    PaletteManager* Manager();

protected:
    PaletteManager* manager_;

    QPixmap bg_pix_;
};

#endif // PALETE_WIDGET_H
