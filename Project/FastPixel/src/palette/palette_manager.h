#ifndef COLOR_PALETTE_MANAGER_H
#define COLOR_PALETTE_MANAGER_H

#include "palette_enum.h"

#include <QColor>
#include <QObject>

class PaleteWidget;
class PaletteBtns;
class PaletteActivation;
class QSplitter;

class PaletteManager : public QObject
{
    Q_OBJECT
public:
    PaletteManager();
    ~PaletteManager();

    bool ContainsColor(ColorType c_type) const;
    bool ContainsColor(QColor color) const;

    int GetColorId(ColorType c_type) const;
    QColor GetColor(ColorType c_type) const;
    Colors GetColors() const;
    int GetColorsSize() const;

    QWidget* GetActivationWid();
    QWidget* GetPaleteWid(int id);
    QWidget* GetMenuWid();
    QWidget* GetSelectWid();

    void ChangePColor(const QColor& color);
    void ChangeSColor(const QColor& color);

    void InitColor();

Q_SIGNALS:
    void SgnChangePSColor(const QString& pcolor, const QString& scolor);

private:
    void ChangeColor(ColorType c_type, const QColor& color);
    void ChangeColorById(ColorType c_type, const int& color_id);
    void AddColor(ColorType c_type);

private:
    Colors colors_;

    QColor primary_color_;
    QColor secondary_color_;

    int primary_id_;
    int secondary_id_;

    QWidget* palete_menu_wid_; // 调色盘属性设置窗口
    QSplitter* palete_select_wid_; // 调色盘选择窗口

    PaletteActivation* activation_wid_;
    QList<PaleteWidget*> palete_wids_;
};

#endif // COLOR_PALETTE_MANAGER_H
