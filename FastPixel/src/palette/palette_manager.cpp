#include "palette_manager.h"

#include "palete_widget.h"
#include "palette_activation.h"
#include "palette_btns.h"
#include "palette_panel_crgb_v.h"
#include "palette_panel_hs_v.h"
#include "palette_panel_sv_h.h"

#include <QComboBox>
#include <QSplitter>

PaletteManager::PaletteManager()
  : activation_wid_(new PaletteActivation())

{
    activation_wid_->SetManager(this);
    colors_ << Qt::black << Qt::white << Qt::lightGray << Qt::red << Qt::cyan << Qt::green;

    // btn
    PaleteWidget* wid = new PaletteBtns();
    wid->setObjectName("Btns");
    palete_wids_ << wid;

    // hsv
    wid = new PalettePanel_HS_V();
    wid->setObjectName("HS_V");
    palete_wids_ << wid;

    // svh
    wid = new PalettePanel_SV_H();
    wid->setObjectName("SV_H");
    palete_wids_ << wid;

    // crgbh
    wid = new PalettePanel_CRGB_V();
    wid->setObjectName("CRGB_V");
    palete_wids_ << wid;

    foreach (auto wid, palete_wids_) {
        wid->SetManager(this);
        connect(wid, &PaleteWidget::SgnChangeColor,
                this, &PaletteManager::ChangeColor);
        connect(wid, &PaleteWidget::SgnSelectColor,
                this, &PaletteManager::ChangeColorById);
    }

    connect(activation_wid_, &PaletteActivation::SgnAddColor,
            this, &PaletteManager::AddColor);

    // 调色盘选择窗口
    palete_select_wid_ = new QSplitter;
    palete_select_wid_->setOrientation(Qt::Vertical);
    palete_select_wid_->addWidget(GetPaleteWid(0));
    palete_select_wid_->addWidget(GetPaleteWid(1));
    palete_select_wid_->addWidget(GetPaleteWid(2));
    palete_select_wid_->addWidget(GetPaleteWid(3));
    palete_select_wid_->setCollapsible(0, false);
    palete_select_wid_->setCollapsible(1, false);
    palete_select_wid_->setCollapsible(2, false);
    palete_select_wid_->setCollapsible(3, false);
    palete_select_wid_->widget(2)->hide();
    palete_select_wid_->widget(3)->hide();

    // 调色盘属性设置窗口
    palete_menu_wid_ = new QWidget;
    palete_menu_wid_->setFixedHeight(20);
    QComboBox* box = new QComboBox(palete_menu_wid_);
    box->addItem("HS_V", 1);
    box->addItem("SV_H", 2);
    box->addItem("CRGB_V", 3);
    connect(box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [&](int data) {
                QComboBox* box = qobject_cast<QComboBox*>(sender());
                const int id = box->currentData().toInt();
                palete_select_wid_->widget(1)->hide();
                palete_select_wid_->widget(2)->hide();
                palete_select_wid_->widget(3)->hide();
                palete_select_wid_->widget(id)->show();
                palete_select_wid_->setSizes(QList<int> { 400, 400, 400, 400 });
            });

    box->setCurrentIndex(0);
}

PaletteManager::~PaletteManager()
{
}

bool PaletteManager::ContainsColor(ColorType c_type) const
{
    switch (c_type) {
    case K_CT_Primary: {
        return colors_.contains(primary_color_);
    }
    case K_CT_Secondary: {
        return colors_.contains(secondary_color_);
    }
    default: {
        return false;
    }
    }
}

bool PaletteManager::ContainsColor(QColor color) const
{
    return colors_.contains(color);
}

// 获取当前颜色id
int PaletteManager::GetColorId(ColorType c_type) const
{
    switch (c_type) {
    case K_CT_Primary: {
        return primary_id_;
    }
    case K_CT_Secondary: {
        return secondary_id_;
    }
    default: {
        return -1;
    }
    }
}

// 获取当前颜色
QColor PaletteManager::GetColor(ColorType c_type) const
{
    switch (c_type) {
    case K_CT_Primary: {
        return primary_color_;
    }
    case K_CT_Secondary: {
        return secondary_color_;
    }
    default: {
        return Qt::transparent;
    }
    }
}

Colors PaletteManager::GetColors() const
{
    return colors_;
}

int PaletteManager::GetColorsSize() const
{
    return colors_.size();
}

QWidget* PaletteManager::GetActivationWid()
{
    return activation_wid_;
}

QWidget* PaletteManager::GetPaleteWid(int id)
{
    if (id >= 0 && id < palete_wids_.size()) {
        return palete_wids_.at(id);
    }
    return nullptr;
}

QWidget* PaletteManager::GetMenuWid()
{
    return palete_menu_wid_;
}

QWidget* PaletteManager::GetSelectWid()
{
    return palete_select_wid_;
}

void PaletteManager::ChangePColor(const QColor& color)
{
    primary_color_ = color;
    emit SgnChangePSColor(primary_color_.name(), secondary_color_.name());
}

void PaletteManager::ChangeSColor(const QColor& color)
{
    secondary_color_ = color;
    emit SgnChangePSColor(primary_color_.name(), secondary_color_.name());
}

void PaletteManager::InitColor()
{
    ChangeColorById(K_CT_Primary, 0);
    ChangeColorById(K_CT_Secondary, 1);
}

// 切换颜色
void PaletteManager::ChangeColor(ColorType c_type, const QColor& color)
{
    switch (c_type) {
    case K_CT_Primary: {
        ChangePColor(color);
        primary_id_ = ContainsColor(color) ? primary_id_ : -1;
    } break;
    case K_CT_Secondary: {
        ChangeSColor(color);
        secondary_id_ = ContainsColor(color) ? secondary_id_ : -1;
    } break;
    default: {

    } break;
    }

    activation_wid_->UpdataCurColor(c_type);
    foreach (auto wid, palete_wids_) {
        wid->UpdataCurColor(c_type);
    }
}

void PaletteManager::ChangeColorById(ColorType c_type, const int& color_id)
{
    if (color_id >= colors_.size()) {
        return;
    }

    switch (c_type) {
    case K_CT_Primary: {
        ChangePColor(colors_.at(color_id));
        primary_id_ = color_id;
    } break;
    case K_CT_Secondary: {
        ChangeSColor(colors_.at(color_id));
        secondary_id_ = color_id;
    } break;
    default: {

    } break;
    }

    activation_wid_->UpdataCurColor(c_type);
    foreach (auto wid, palete_wids_) {
        wid->UpdataCurColor(c_type);
    }
}

// 增加新颜色
void PaletteManager::AddColor(ColorType c_type)
{
    switch (c_type) {
    case K_CT_Primary: {
        colors_ << primary_color_;
    } break;
    case K_CT_Secondary: {
        colors_ << secondary_color_;
    } break;
    default: {

    } break;
    }

    activation_wid_->UpdataCurColor(c_type);
    foreach (auto wid, palete_wids_) {
        wid->UpdataCurColor(c_type);
    }
}
