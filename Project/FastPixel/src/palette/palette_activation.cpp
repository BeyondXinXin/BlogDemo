#include "palette_activation.h"

#include "palette_manager.h"

#include <QDebug>

PaletteActivation::PaletteActivation(QWidget *parent)
  : QWidget(parent)
  , primary_lab_(new QLabel(this))
  , secondary_lab_(new QLabel(this))
  , primary_btn_(new QPushButton(this))
  , secondary_btn_(new QPushButton(this))
  , space_(0)
  , manager_(nullptr)
{
    setWindowFlags(Qt::WindowCloseButtonHint);

    primary_btn_->setText(" + ");
    secondary_btn_->setText(" + ");
    primary_btn_->setFixedSize(35, 40);
    secondary_btn_->setFixedSize(35, 40);
    primary_btn_->setVisible(false);
    secondary_btn_->setVisible(false);

    primary_lab_->setAlignment(Qt::AlignCenter);
    secondary_lab_->setAlignment(Qt::AlignCenter);

    connect(primary_btn_, &QPushButton::clicked, this, [&] {
        emit SgnAddColor(K_CT_Primary);
        primary_btn_->setVisible(false);
    });

    connect(secondary_btn_, &QPushButton::clicked, this, [&] {
        emit SgnAddColor(K_CT_Secondary);
        secondary_btn_->setVisible(false);
    });

    setFixedHeight(80);
}

PaletteActivation::~PaletteActivation()
{
}

void PaletteActivation::SetManager(PaletteManager *manager)
{
    manager_ = manager;
}

void PaletteActivation::UpdataCurColor(ColorType c_type)
{
    if (!manager_) {
        return;
    }

    static const QString sheet_str = QString("QLabel{font:25px;color:%1;background:%2;}");

    QColor color = manager_->GetColor(c_type);
    int temp = (color.red() + color.green() + color.blue()) / 3;
    QString name = color.name().toUpper();
    QString sheet = sheet_str.arg(temp < 200 ? "#FEFEFE" : "#000000").arg(color.name());
    bool addbtn_show = !manager_->ContainsColor(c_type);

    switch (c_type) {
    case K_CT_Primary: {
        primary_lab_->setText(name);
        primary_lab_->setStyleSheet(sheet);
        primary_btn_->setVisible(addbtn_show);
    } break;
    case K_CT_Secondary: {
        secondary_lab_->setText(name);
        secondary_lab_->setStyleSheet(sheet);
        secondary_btn_->setVisible(addbtn_show);
    } break;
    default:
        break;
    }

    resizeEvent(nullptr);
}

void PaletteActivation::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const int w_addbtn_show = static_cast<int>(width() - 2 * space_ - 35 - space_);
    const int w_addbtn_hide = static_cast<int>(width() - 2 * space_);
    const int h = static_cast<int>((height() - 3 * space_) / 2);
    const QSize primary_btn_size = primary_btn_->isVisible() ? QSize(w_addbtn_show, h) : QSize(w_addbtn_hide, h);
    const QSize secondary_btn_size = secondary_btn_->isVisible() ? QSize(w_addbtn_show, h) : QSize(w_addbtn_hide, h);

    primary_lab_->resize(primary_btn_size);
    secondary_lab_->resize(secondary_btn_size);

    const int offset_y = 2 * space_ + primary_lab_->height();
    const int p_offset_x = 2 * space_ + primary_lab_->width();
    const int s_offset_x = 2 * space_ + secondary_lab_->width();

    primary_lab_->move(space_, space_);
    primary_btn_->move(p_offset_x, space_);

    secondary_lab_->move(space_, offset_y);
    secondary_btn_->move(s_offset_x, offset_y);
}
